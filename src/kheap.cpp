
#include "system.h"
#include "kheap.h"

#include "debug.h"
#include "mm.h"
#include "mmdef.h"

enum HeapObjectStatus
{
	HEAP_OBJECT_FREE = 0x11111111,	
	HEAP_OBJECT_USED = 0xFFFFFFFF,
	HEAP_OBJECT_INIT = 0x33333333,
};

// Placement new, we need this :>
void* operator new( size_t, void* ptr )
{
	return ptr;
}

void*	operator new(size_t size)
{
	TRACE1(size);
	return mm::heap::alloc(size);
}

void* operator new(size_t size, std::align_val_t)
{
	TRACE1(size);
	return mm::heap::alloc(size);
}

void*	operator new[](size_t size)
{
	TRACE1(size);
	return mm::heap::alloc(size);
}

void operator delete(void* ptr)
{
	TRACE1(ptr);
	return mm::heap::free(ptr);
}

void operator delete(void* ptr, std::align_val_t)
{
	TRACE1(ptr);
	return mm::heap::free(ptr);
}

void operator delete(void* ptr, long unsigned int)
{
	TRACE1(ptr);
	return mm::heap::free(ptr);
}

void operator delete[](void* ptr)
{
	TRACE1(ptr);
	return mm::heap::free(ptr);
}

void operator delete[](void* ptr, long unsigned int)
{
	TRACE1(ptr);
	return mm::heap::free(ptr);
}

template<unsigned int obj_size>
struct heap_object
{
	typedef heap_object<obj_size>	TMyType;

private:
	HeapObjectStatus		_status;
	union
	{
		heap_object<obj_size>*	_next;
		size_t					_size;
	};

	unsigned char _data[obj_size];

public:
	heap_object()
		:	_status( HEAP_OBJECT_INIT ),
			_next( 0 )
	{
	}

	heap_object( TMyType* next )
		:	_status( HEAP_OBJECT_FREE ),
			_next( next )
	{
	}

	bool	is_free() const
	{
		return _status == HEAP_OBJECT_FREE;
	}

	bool	is_used() const
	{
		return _status == HEAP_OBJECT_USED;
	}

	size_t	size() const
	{
		return _size;
	}

	void*	alloc( TMyType** pchain )
	{
		*pchain = _next;

		_status = HEAP_OBJECT_USED;
		_size = obj_size;

		return _data;
	}

	void	free( TMyType** pchain )
	{
		_next = *pchain;
		*pchain = this;

		_status = HEAP_OBJECT_FREE;
	}
};

template<unsigned int obj_size, unsigned int page_count>
struct heap_chunk
{
private:
	typedef heap_chunk<obj_size, page_count>	TMyType;
	typedef heap_object<obj_size>				TObject;

	enum
	{
		overhead_size = 8, // change when adding fields to this type
		total_size = (page_count * PAGE_SIZE),
		payload_size = (total_size - overhead_size),
		object_count = payload_size / sizeof(heap_object<obj_size>)
	};

	TMyType*		_next;
	TObject*		_first_free;
	unsigned int	_free_count;
	TObject			_objects[object_count];	

public:
	heap_chunk()
		:	_next( 0 ),
			_first_free( 0 ),
			_free_count( object_count )
	{		
		TObject* next = new (&_objects[object_count-1]) TObject( 0 );		

		for( unsigned int i = object_count - 1; i != 0; --i )
		{
			next = new (&_objects[i-1]) TObject( next );
		}

		_first_free = next;
	}

	void*	alloc()
	{
		void* ptr = _first_free->alloc( &_first_free );

		--_free_count;

		//debug_bochs_printf( "allocated mem from chunk (obj_size = %d, object_count = %d) @ %x, new first_free = %x\n", obj_size, object_count, ptr, _first_free );
		return ptr;
	}

	void	free( void* ptr )
	{
		TObject* obj = reinterpret_cast<TObject*>( uintptr_t(ptr) - sizeof(heap_object<0>) );
		obj->free( &_first_free );

		++_free_count;
		
		//debug_bochs_printf( "freed object, _first_free = %x\n", _first_free );
	}

	void	chain( TMyType* next )
	{
		_next = next;
	}

	TMyType*	next()
	{
		return _next;
	}

	bool	full()
	{
		return _first_free == 0;
	}

	bool	empty()
	{
		return _free_count == object_count;
	}
};

struct iheap_chunk_group
{
	virtual size_t	object_size() = 0;
	virtual void*	alloc() = 0;
	virtual void	free( void* ptr ) = 0;
};

template<unsigned int obj_size, unsigned int page_count>
struct heap_chunk_group
	:	public iheap_chunk_group
{
	typedef heap_chunk<obj_size, page_count>	TChunk;

private:
	TChunk*			_first_chunk;

	unsigned int	_chunk_count;
	unsigned int	_free_object_count;	
	unsigned int	_alloc_count;

public:
	heap_chunk_group()
		:	_first_chunk( 0 ),
			_chunk_count( 0 ),
			_free_object_count( 0 ),			
			_alloc_count( 0 )
	{
	}

	virtual void*	alloc()
	{
		if( _first_chunk )
		{
			void* obj = _first_chunk->alloc();
			if( _first_chunk->full() )
			{
				_first_chunk = _first_chunk->next();
			}

			return obj;
		}
		else
		{
			return alloc_from_new();
		}
	}

	virtual void	free( void* ptr )
	{
		// Chunk always starts at the start of the page
		// Multi-page chunks only contain 1 object, so this is always true
		TChunk* chunk = reinterpret_cast<TChunk*>( uintptr_t(ptr) & ~PAGE_MASK );
		bool was_full = chunk->full();

		chunk->free( ptr );

		if( chunk->empty() )
		{
			debug_bochs_printf( "KILL PAGE\n" );

			if( !was_full )
			{
				if( chunk == _first_chunk )
				{
					_first_chunk = chunk->next();
				}
				else
				{
					TChunk* ch = _first_chunk;
					while( ch && ch->next() != chunk )
						ch = ch->next();

					if( ch )
					{
						ch->chain( chunk->next() );
					}
					else
					{
						PANIC( "huh?! where has my half-full chunk gone?!\n" );
					}
				}
			}

			mm::free_pages( chunk, page_count );
		}
		else if( was_full )
		{
			// lock?
			chunk->chain( _first_chunk );
			_first_chunk = chunk;
			// unlock?
		}		
	}

	virtual size_t	object_size()
	{
		return obj_size;
	}

private:
	void*	alloc_from_new()
	{
		void* p = mm::alloc_pages( page_count );
		if( !p ) return 0;

		TChunk* chunk = new (p) TChunk;
		void* obj = chunk->alloc();

		// Don't have to chain a already-full chunk
		if( !chunk->full() )
		{
			// lock?
			chunk->chain( _first_chunk );
			_first_chunk = chunk;
			// unlock?
		}

		return obj;
	}
};

static heap_chunk_group<8, 1>			heap_chunk_group_8;
static heap_chunk_group<16, 1>			heap_chunk_group_16;
static heap_chunk_group<32, 1>			heap_chunk_group_32;
static heap_chunk_group<64, 1>			heap_chunk_group_64;
static heap_chunk_group<128, 1>			heap_chunk_group_128;
static heap_chunk_group<256, 1>			heap_chunk_group_256;
static heap_chunk_group<512, 1>			heap_chunk_group_512;
static heap_chunk_group<1024-16, 1>		heap_chunk_group_1024;
static heap_chunk_group<2048-16, 1>		heap_chunk_group_2048;
static heap_chunk_group<4096-20, 1>		heap_chunk_group_4096;
static heap_chunk_group<8192-20, 2>		heap_chunk_group_8192;
static heap_chunk_group<16384-20, 4>	heap_chunk_group_16384;
static heap_chunk_group<32768-20, 8>	heap_chunk_group_32768;
static heap_chunk_group<65536-20, 16>	heap_chunk_group_65536;
static heap_chunk_group<131072-20, 32>	heap_chunk_group_131072;
static heap_chunk_group<262144-20, 64>	heap_chunk_group_262144;
static heap_chunk_group<524288-20, 128>	heap_chunk_group_524288;

static iheap_chunk_group*	s_heap_chunk_groups[] =
{
	&heap_chunk_group_8,
	&heap_chunk_group_16,
	&heap_chunk_group_32,
	&heap_chunk_group_64,
	&heap_chunk_group_128,
	&heap_chunk_group_256,
	&heap_chunk_group_512,
	&heap_chunk_group_1024,
	&heap_chunk_group_2048,
	&heap_chunk_group_4096,
	&heap_chunk_group_8192,
	&heap_chunk_group_16384,
	&heap_chunk_group_32768,
	&heap_chunk_group_65536,
	&heap_chunk_group_131072,
	&heap_chunk_group_262144,
	&heap_chunk_group_524288,
};

static iheap_chunk_group*	find_chunk_group( size_t request_size )
{
	for( unsigned int i = 0; i < _countof(s_heap_chunk_groups); ++i )
	{
		if( s_heap_chunk_groups[i]->object_size() >= request_size )
		{
			return s_heap_chunk_groups[i];
		}
	}

	return 0;
}

void	mm::heap::init()
{
}

void*	mm::heap::alloc( size_t size )
{
	auto chunk_group = find_chunk_group( size );
	if( chunk_group )
		return chunk_group->alloc();

	return 0;
}

void	mm::heap::free( void* ptr )
{
	heap_object<0>* obj = reinterpret_cast<heap_object<0>*>( uintptr_t(ptr) - sizeof(heap_object<0>) );
	if( obj->is_used() )
	{
		auto group = find_chunk_group( obj->size() );
		if( group )
		{
			return group->free( ptr );			
		}
	}
	
	debug_bochs_printf( "Trying to free non-allocated pointer (%x)\n", ptr );
}

void	mm::heap::stat()
{
}

