
#include "system.h"
#include "pmem.h"

#include "paging.h"
#include "multiboot.h"

#include "elf.h"

#include "debug.h"
#include "console.h"
extern CONSOLE console;

const	size_t	PAGE_SIZE = 0x1000;

const	size_t	MAXIMUM_MEMORY = 1024*1024*512;
const	size_t	MAXIMUM_PAGEFRAME_COUNT = MAXIMUM_MEMORY / PAGE_SIZE;

static	size_t	s_physical_memory_size = 0;
static	size_t	s_actual_pageframe_count = 0;

static int s_pageframes[MAXIMUM_PAGEFRAME_COUNT] = {};
static int s_prev_index = 0;

static void pmem_set_status( uintptr_t physical_addr, bool free )
{
	int idx = physical_addr / PAGE_SIZE;
	int& entry = s_pageframes[idx];

	entry = free ? 0 : 1;
}

uintptr_t pmem_alloc()
{
	for( size_t i = 0; i < s_actual_pageframe_count; ++i )
	{
		int idx = (s_prev_index + i) % s_actual_pageframe_count;
		int& entry = s_pageframes[idx];

		if( entry == 0 )
		{
			entry = 1;

			s_prev_index = idx;
			return idx * PAGE_SIZE;
		}
	}

	debug_bochs_printf( "physmem_alloc_pageframe: OUT OF MEMORY\n" );

	return 0;
}

void pmem_free( uintptr_t physical_addr )
{
	pmem_set_status( physical_addr, true );
}

void pmem_mark_used( uintptr_t physical_addr )
{
	pmem_set_status( physical_addr, false );
}



static void pmem_mark_free( uintptr_t physical_addr, size_t size )
{
	for( unsigned int i = 0; i < size; i += PAGE_SIZE )
	{
		pmem_set_status( physical_addr + i, true );
	}
}

void pmem_init( multiboot_info_t* mbt )
{
	debug_bochs_printf( "mbt->mmap_length = %x\n", mbt->mmap_length );
	debug_bochs_printf( "mbt->mmap_addr = %x\n", mbt->mmap_addr );

	memory_map_t* mmap = (memory_map_t*)mbt->mmap_addr;
	
	uintptr_t memory_end = 0;

	// Initially set all pageframes to not-available
	for( size_t i = 0; i < MAXIMUM_MEMORY; i += PAGE_SIZE )
	{
		pmem_set_status( i, false );
	}

	while((unsigned int)mmap < mbt->mmap_addr + mbt->mmap_length)
	{
		if( mmap->type == 0x1 )
		{
			pmem_mark_free( mmap->base_addr_low, mmap->length_low );

			console.printf( "Found usable memory %x - %x\n", mmap->base_addr_low, mmap->base_addr_low + mmap->length_low );			

			uintptr_t block_end = mmap->base_addr_low + mmap->length_low;
			if( block_end > memory_end )
			{
				memory_end = block_end;
			}
		}

		mmap = (memory_map_t*) ( (unsigned int)mmap + mmap->size + sizeof(unsigned int) );
	}

	if( memory_end )
	{
		debug_bochs_printf( "Detected %d MiB of memory\n", memory_end / (1024 * 1024) );

		s_physical_memory_size = memory_end;
		s_actual_pageframe_count = s_physical_memory_size / PAGE_SIZE;
	}
	else
	{
		debug_bochs_printf( "physmem_init_from_mbt: NO MEMORY FOUND!\n" );
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
					pmem_mark_used( physical_address );

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
	pmem_mark_used( 0 );
}
