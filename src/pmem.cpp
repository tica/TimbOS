
#include "system.h"
#include "pmem.h"

#include "paging.h"
#include "multiboot.h"

#include "elf.h"

#include "debug.h"
#include "console.h"
extern CONSOLE console;

const	size_t	PAGE_SIZE = 0x1000;

const	size_t	DMA_MEMORY_LIMIT = 16 * 1024 * 1024;
const	size_t	MAXIMUM_DMA_PAGEFRAME_COUNT = DMA_MEMORY_LIMIT / PAGE_SIZE;

const	size_t	MAXIMUM_MEMORY = 1024*1024*512;
const	size_t	MAXIMUM_PAGEFRAME_COUNT = MAXIMUM_MEMORY / PAGE_SIZE;

static	size_t	s_physical_memory_size = 0;

static	size_t	s_dma_pageframe_count = 0;
static	int		s_dma_pageframes[MAXIMUM_DMA_PAGEFRAME_COUNT] = {};

static	size_t			s_higher_pageframe_count = 0;
static	int				s_higher_pageframes[(MAXIMUM_PAGEFRAME_COUNT - MAXIMUM_DMA_PAGEFRAME_COUNT)] = {};


class PhysicalPool
{
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
	PhysicalPool()
	{
	}

	void init( uintptr_t physical_base, uintptr_t physical_limit, int* bitmap )
	{
		_base = physical_base;
		_num_pageframes = (physical_limit - physical_base) / PAGE_SIZE;
		_prev_idx = 0;
		_bitmap = bitmap;
	}

	uintptr_t	alloc()
	{
		return alloc( 1, 1 );
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

				_prev_idx = (idx + pages) % _num_pageframes;
				return address_of(idx);
			}
		}

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

	size_t	size()
	{
		return _num_pageframes * PAGE_SIZE;
	}
};

static PhysicalPool DefaultPool;
static PhysicalPool DMAPool;


uintptr_t	pmem_alloc()
{
	auto p = DefaultPool.alloc();

	if( !p )
		p = DMAPool.alloc( 1, 1 );

	if( !p )
		debug_bochs_printf( "pmem_alloc: OUT OF MEMORY\n" );

	return p;
}

uintptr_t	pmem_alloc_dma(unsigned int pages, unsigned int align)
{
	auto p = DMAPool.alloc(pages, align);

	if( !p )
		debug_bochs_printf( "pmem_alloc_dma: OUT OF MEMORY\n" );

	return p;
}

void pmem_free( uintptr_t physical_addr )
{
	if( physical_addr < DMA_MEMORY_LIMIT )
	{
		DMAPool.free( physical_addr );
	}
	else
	{
		DefaultPool.free( physical_addr );
	}
}

void pmem_reserve( uintptr_t physical_addr )
{
	if( physical_addr < DMA_MEMORY_LIMIT )
	{
		DMAPool.reserve( physical_addr );
	}
	else
	{
		DefaultPool.reserve( physical_addr );
	}
}


static void pmem_mark_free_( uintptr_t physical_addr )
{
	if( physical_addr < DMA_MEMORY_LIMIT )
	{
		DMAPool.mark_free( physical_addr );
	}
	else
	{
		DefaultPool.mark_free( physical_addr );
	}
}

static void pmem_mark_free( uintptr_t physical_addr, size_t size )
{
	for( unsigned int i = 0; i < size; i += PAGE_SIZE )
	{
		pmem_mark_free_( physical_addr + i );
	}
}

uintptr_t	find_memory_end( multiboot_info_t* mbt )
{
	uintptr_t memory_end = 0;

	memory_map_t* mmap = (memory_map_t*)mbt->mmap_addr;
	while((unsigned int)mmap < mbt->mmap_addr + mbt->mmap_length)
	{
		if( mmap->type == 0x1 )
		{
			uintptr_t block_end = mmap->base_addr_low + mmap->length_low;

			if( block_end > memory_end )
				memory_end = block_end;
		}

		mmap = (memory_map_t*) ( (unsigned int)mmap + mmap->size + sizeof(unsigned int) );
	}

	return memory_end;
}

void pmem_init( multiboot_info_t* mbt )
{
	debug_bochs_printf( "mbt->mmap_length = %x\n", mbt->mmap_length );
	debug_bochs_printf( "mbt->mmap_addr = %x\n", mbt->mmap_addr );

	uintptr_t memory_end = find_memory_end( mbt );
	if( !memory_end )
	{
		PANIC( "NO MEMORY FOUND!\n" );
	}

	debug_bochs_printf( "Detected %d MiB of memory\n", memory_end / (1024 * 1024) );

	s_physical_memory_size = memory_end;

	if( s_physical_memory_size > DMA_MEMORY_LIMIT )
	{
		s_dma_pageframe_count = MAXIMUM_DMA_PAGEFRAME_COUNT;
		s_higher_pageframe_count = (s_physical_memory_size / PAGE_SIZE) - s_dma_pageframe_count;
	}
	else
	{
		s_dma_pageframe_count = (s_physical_memory_size / PAGE_SIZE);
		s_higher_pageframe_count = 0;
	}

	DMAPool.init( 0, DMA_MEMORY_LIMIT, s_dma_pageframes );
	DefaultPool.init( DMA_MEMORY_LIMIT, s_physical_memory_size, s_higher_pageframes );

	debug_bochs_printf( "Assigning %d MiB as possible DMA memory, %d MiB possible default memory\n", DMAPool.size(), DefaultPool.size() );
	
	
	// Initially set all pageframes to not-available
	for( uintptr_t i = 0; i < MAXIMUM_MEMORY; i += PAGE_SIZE )
	{
		pmem_reserve( i );
	}

	memory_map_t* mmap = (memory_map_t*)mbt->mmap_addr;
	while((unsigned int)mmap < mbt->mmap_addr + mbt->mmap_length)
	{
		if( mmap->type == 0x1 )
		{
			console.printf( "Found usable memory %x - %x\n", mmap->base_addr_low, mmap->base_addr_low + mmap->length_low );
			pmem_mark_free( mmap->base_addr_low, mmap->length_low );			
		}

		mmap = (memory_map_t*) ( (unsigned int)mmap + mmap->size + sizeof(unsigned int) );
	}
	

	if( mbt->flags & (1 << 5) )
	{
		const elf_section_header_table& esht = mbt->u.elf_sec;

		/*
		debug_bochs_printf( "mbt->u.elf_sec.addr = %x\n", esht.addr );
		debug_bochs_printf( "mbt->u.elf_sec.num = %x\n", esht.num );
		debug_bochs_printf( "mbt->u.elf_sec.shndx = %x\n", esht.shndx );
		debug_bochs_printf( "mbt->u.elf_sec.size = %x\n", esht.size );
		*/

		Elf32_Shdr* esh = (Elf32_Shdr*)esht.addr;

		for( size_t i = 0; i < esht.num; ++i )
		{
			if( esh->sh_flags & SHF_ALLOC )
			{
				/*
				console.printf( "ELF section: addr=%x size=%x writable=%s executable=%s\n",
					esh->sh_addr, esh->sh_size, esh->sh_flags & SHF_WRITE ? "yes" : "no",
					esh->sh_flags & SHF_EXECINSTR ? "yes" : "no" );
				*/

				size_t size = esh->sh_size;
				int pgindex = (esh->sh_addr >> 12) & 0x3FF;
				uintptr_t virtual_addr = esh->sh_addr;

				while( size > 0 )
				{
					uintptr_t physical_address = KernelPageDirectory.virtual_to_physical( virtual_addr );
					pmem_reserve( physical_address );

					pgindex++;

					unsigned int page_step = 0x1000 - (virtual_addr % 0x1000);
					if( page_step > size ) page_step = size;

					virtual_addr += page_step;
					size -= page_step;
				}
			}
			esh++;
		}
	}

	// Do not let 0x00000000 be a valid address
	pmem_reserve( 0 );
}

