
#include "system.h"
#include "physical.h"

#include "mmdef.h"
#include "mbtutil.h"

#include "paging.h"
#include "multiboot.h"

#include "elf.h"

#include "debug.h"
#include "console.h"
extern CONSOLE console;

class PhysicalPool
{
	PhysicalPool*	_fallback;

	uintptr_t		_base;
	
	unsigned int	_num_pageframes;
	unsigned int	_prev_idx;
	int*			_bitmap;

private:
	int&	status( unsigned int idx )
	{
		return _bitmap[idx];
	}

	uintptr_t	address_of( unsigned int idx )
	{
		return _base + idx * PAGE_SIZE;
	}

	unsigned int	index_of( uintptr_t addr )
	{
		return (addr - _base) / PAGE_SIZE;
	}

public:
	PhysicalPool( PhysicalPool* fallback )
		:	_fallback( fallback )
	{
	}

	void init( uintptr_t physical_base, size_t num_pageframes, int* bitmap )
	{
		_base = physical_base;
		_num_pageframes = num_pageframes,
		_prev_idx = 0;
		_bitmap = bitmap;

		for( unsigned int i = 0; i < _num_pageframes; ++i )
		{
			status(i) = 0;
		}
	}

	uintptr_t	alloc( unsigned int pages, unsigned int alignment )
	{
		size_t i = 0;
		while( (i + _prev_idx) % alignment )
			++i;
	
		for( ; i < _num_pageframes; i += alignment )
		{
			bool ok = true;
			for( unsigned int j = 0; j < pages; ++j )
			{
				int idx = (_prev_idx + i + j) % _num_pageframes;
				if( status(idx) )
				{
					ok = false;
					break;
				}
			}

			if( ok )
			{
				int idx = (_prev_idx + i) % _num_pageframes;
				for( unsigned int j = 0; j < pages; ++j )
				{
					status(idx + j) = 1;
				}

				_prev_idx = (idx + pages) % _num_pageframes;

				return address_of(idx);
			}
		}

		if( _fallback )
			return _fallback->alloc( pages, alignment );

		return 0;
	}

	void	free( uintptr_t addr )
	{
		status( index_of(addr) ) = 0;
	}

	void	mark_free( uintptr_t addr )
	{
		status( index_of(addr) ) = 0;
	}

	void	reserve( uintptr_t addr )
	{
		status( index_of(addr) ) = 1;
	}

	uintptr_t	base()
	{
		return _base;
	}

	size_t	size()
	{
		return _num_pageframes * PAGE_SIZE;
	}

	uintptr_t	limit()
	{
		return _base + size();
	}
};

static PhysicalPool DMAPool( 0 );
static PhysicalPool DefaultPool( &DMAPool );
static PhysicalPool HiMemPool( &DefaultPool );

uintptr_t	mm::physical::alloc_dma(unsigned int pages, unsigned int align)
{
	auto p = DMAPool.alloc(pages, align);

	if( !p )
		debug_bochs_printf( "mm::physical::alloc_dma: OUT OF MEMORY\n" );

	return p;
}

uintptr_t	mm::physical::alloc(unsigned int pages, unsigned int align)
{
	auto p = DefaultPool.alloc(pages, align);

	if( !p )
		debug_bochs_printf( "mm::physical::alloc: OUT OF MEMORY\n" );

	return p;
}

uintptr_t	mm::physical::alloc_himem(unsigned int pages, unsigned int align)
{
	auto p = HiMemPool.alloc(pages, align);

	if( !p )
		debug_bochs_printf( "mm::physical::alloc_himem: OUT OF MEMORY\n" );

	return p;
}

PhysicalPool* find_pool( uintptr_t physical_addr )
{
	if( physical_addr < DMAPool.limit() )
	{
		return &DMAPool;
	}
	else if( physical_addr < DefaultPool.limit() )
	{
		return &DefaultPool;
	}
	else if( physical_addr < HiMemPool.limit() )
	{
		return &HiMemPool;
	}

	return 0;
}

void mm::physical::free( uintptr_t physical_addr )
{
	auto pool = find_pool( physical_addr );
	if( pool )
		pool->free( physical_addr );
}

void reserve( uintptr_t physical_addr )
{
	auto pool = find_pool( physical_addr );
	if( pool )
		pool->reserve( physical_addr );
}

void reserve_range( uintptr_t physical_addr, size_t size )
{
	for( unsigned int i = 0; i < size; i += PAGE_SIZE )
	{
		reserve( physical_addr + i );
	}
}


template<typename T>
static T* linear_alloc( size_t size, size_t align, uintptr_t* pos )
{
	while( *pos & align )
		++*pos;

	T* obj = reinterpret_cast<T*>( *pos );
	*pos += size;

	return obj;
}

void mm::physical::init( const multiboot_info* mbt, uintptr_t kernel_static_end, uintptr_t kernel_dynamic_end, uintptr_t himem_end )
{
	if( !himem_end )
	{
		PANIC( "NO MEMORY FOUND!\n" );
	}	
	
	size_t dma_pool_size = (DMA_MEMORY_LIMIT < kernel_dynamic_end) ? DMA_MEMORY_LIMIT : kernel_dynamic_end;	
	size_t default_pool_size = kernel_dynamic_end - dma_pool_size;
	size_t himem_pool_size = himem_end - kernel_dynamic_end;

	size_t dma_pool_pages = dma_pool_size / PAGE_SIZE;
	size_t default_pool_pages = default_pool_size / PAGE_SIZE;
	size_t himem_pool_pages = himem_pool_size / PAGE_SIZE;

	int* dma_pool_bitmap = linear_alloc<int>( dma_pool_pages * sizeof(int), 16, &kernel_static_end );
	int* default_pool_bitmap = linear_alloc<int>( default_pool_pages * sizeof(int), 16, &kernel_static_end );
	int* himem_pool_bitmap = linear_alloc<int>( himem_pool_pages * sizeof(int), 16, &kernel_static_end );

	uintptr_t dma_pool_start = 0;
	uintptr_t default_pool_start = dma_pool_start + dma_pool_size;
	uintptr_t himem_pool_start = default_pool_start + default_pool_size;

	DMAPool.init( dma_pool_start, dma_pool_pages, dma_pool_bitmap );
	DefaultPool.init( default_pool_start, default_pool_pages, default_pool_bitmap );
	HiMemPool.init( himem_pool_start, himem_pool_pages, himem_pool_bitmap );

	debug_bochs_printf( "Physical memory layout:\n" );
	debug_bochs_printf( "  DMA memory     %x-%x\n", DMAPool.base(), DMAPool.limit() );
	debug_bochs_printf( "  Default memory %x-%x\n", DefaultPool.base(), DefaultPool.limit() );
	debug_bochs_printf( "  High memory    %x-%x\n", HiMemPool.base(), HiMemPool.limit() );
	
	multiboot::iterate_mmap( mbt,
		[](const multiboot_mmap_entry* mmap)
		{
			if( !(mmap->type & MULTIBOOT_MEMORY_AVAILABLE) )
			{
				//console.printf( "Found reserved memory %lx - %lx\n", mmap->addr, mmap->addr + mmap->len );
				reserve_range( mmap->addr, mmap->len );
			}
		}
	);

	// Reserve kernel code + pmem bitmaps (added above to kernel_static_end)
	reserve_range( 0, kernel_static_end );	
}

