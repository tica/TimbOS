
#include "physmem.h"

#include "debug.h"
#include "console.h"
extern CONSOLE console;

const size_t PAGE_SIZE = 0x1000;

const size_t MAXIMUM_MEMORY = 1024*1024*512;
const size_t PAGEFRAMES_PER_BITMAP_ENTRY = sizeof(int) * 8;
const size_t PAGEFRAME_BITMAP_SIZE = MAXIMUM_MEMORY / (PAGEFRAMES_PER_BITMAP_ENTRY * PAGE_SIZE);

int s_physmem_allocation_bitmap[PAGEFRAME_BITMAP_SIZE] = { 0 };


uintptr_t physmem_alloc_pageframe()
{
	for( size_t i = 0; i < PAGEFRAME_BITMAP_SIZE; ++i )
	{
		int& entry = s_physmem_allocation_bitmap[i];

		if( entry != ~0 )
		{			
			for( size_t j = 0; j < PAGEFRAMES_PER_BITMAP_ENTRY; ++j )
			{
				int bitmask = 1 << j;

				if( entry & bitmask )
				{
					entry &= ~bitmask;

					return (i * PAGEFRAMES_PER_BITMAP_ENTRY + j) * PAGE_SIZE;
				}
			}
		}
	}

	debug_bochs_printf( "physmem_alloc_pageframe: OUT OF MEMORY\n" );
	return 0;
}

void physmem_set_pageframe_status( uintptr_t physical_addr, bool free )
{
	size_t index = (physical_addr >> 12) / PAGEFRAMES_PER_BITMAP_ENTRY;
	//assert( index < PAGEFRAME_BITMAP_SIZE)

	int bit = (physical_addr >> 12) % PAGEFRAMES_PER_BITMAP_ENTRY;	

	if( free )
		s_physmem_allocation_bitmap[index] |= (1 << bit);
	else
		s_physmem_allocation_bitmap[index] &= ~(1 << bit);
}

void physmem_free_pageframe( uintptr_t physical_addr )
{
	physmem_set_pageframe_status( physical_addr, true );
}

void physmem_init_from_mbt( multiboot_info_t* mbt )
{
	debug_bochs_printf( "mbt->mmap_length = %x\n", mbt->mmap_length );
	debug_bochs_printf( "mbt->mmap_addr = %x\n", mbt->mmap_addr );

	memory_map_t* mmap = (memory_map_t*)mbt->mmap_addr;

	uintptr_t main_memory = 0;
	size_t main_memory_size = 0;

	while((unsigned int)mmap < mbt->mmap_addr + mbt->mmap_length)
	{
		if( mmap->type == 0x1 )
		{
			console.printf( "Found usable memory %x - %x\n", mmap->base_addr_low, mmap->base_addr_low + mmap->length_low );

			if( mmap->base_addr_low == 0x00100000 )
			{
				main_memory = mmap->base_addr_low;
				main_memory_size = mmap->length_low;
			}
		}

		mmap = (memory_map_t*) ( (unsigned int)mmap + mmap->size + sizeof(unsigned int) );
	}

	if( main_memory )
	{
		debug_bochs_printf( "Detected main memory @ %x, size = %x\n", main_memory, main_memory_size );

		for( size_t i = 0; i < main_memory_size; i += PAGE_SIZE )
		{
			physmem_set_pageframe_status( main_memory + i, true );
		}
	}
	else
	{
		debug_bochs_printf( "physmem_init_from_mbt: MAIN MEMORY NOT FOUND @ 0x00100000\n" );
	}
}
