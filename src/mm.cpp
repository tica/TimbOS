
#include "mm.h"

#include "mmdef.h"
#include "physical.h"
#include "mbtutil.h"
#include "paging.h"
#include "kheap.h"

#include "multiboot.h"
#include "debug.h"

#include "console.h"
extern CONSOLE console;

uintptr_t	find_memory_end( const multiboot_info* mbt )
{
	uintptr_t memory_end = 0;

	multiboot::iterate_mmap( mbt,
		[&memory_end](const multiboot_mmap_entry* mmap)
		{
			if( mmap->type == MULTIBOOT_MEMORY_AVAILABLE )
			{
				// FIXME: 64-32 bit truncation, bad bad bad.
				uintptr_t block_end = mmap->addr + mmap->len;

				if( block_end > memory_end )
					memory_end = block_end;
			}
		}
	);

	return memory_end;
}

uintptr_t	find_kernel_static_end( const multiboot_info* mbt )
{
	uintptr_t kernel_end = 0;

	multiboot::iterate_elf_section_headers( mbt, SHF_ALLOC,
		[&kernel_end]( const Elf32_Shdr* esh )
		{
			uintptr_t section_end = esh->sh_addr + esh->sh_size;

			if( section_end > kernel_end )
				kernel_end = section_end;
		}
	);

	return kernel_end - KERNEL_VIRTUAL_BASE;
}

void mm::init( const multiboot_info* mbt_info )
{
	if( mbt_info->flags & MULTIBOOT_MEMORY_INFO )
	{
		console.printf( "Memory (Lower/Upper): %d KiB/%d KiB\n", mbt_info->mem_lower, mbt_info->mem_upper );
	}
	else
	{
		console.printf( "WARNING: MultiBoot header did not specify memory\n" );
	}

	uintptr_t memory_end = find_memory_end( mbt_info );
	uintptr_t kernel_static_end = find_kernel_static_end( mbt_info );

	uintptr_t kernel_dynamic_start = kernel_static_end;
	uintptr_t kernel_dynamic_end = (memory_end > KERNEL_VIRTUAL_SIZE_MAX) ? KERNEL_VIRTUAL_SIZE_MAX : memory_end;

	paging::init( kernel_dynamic_end, KERNEL_VIRTUAL_BASE );

	uintptr_t himem_start = kernel_dynamic_end;
	uintptr_t himem_end = memory_end;
	size_t himem_size = himem_end - himem_start;

	debug_bochs_printf( "mm::init: kernel_dynamic_start = 0x%p\n", kernel_dynamic_start );
	debug_bochs_printf( "mm::init: kernel_dynamic_end = 0x%p\n", kernel_dynamic_end );
	debug_bochs_printf( "mm::init: himem_size = 0x%p\n", himem_size );

	physical::init( mbt_info, kernel_static_end, kernel_dynamic_end, himem_end );
	heap::init();
}

void*	mm::alloc_pages_dma( unsigned int count, unsigned int align )
{
	uintptr_t physical_addr = physical::alloc_dma( count, align );
	if( physical_addr )
	{
		return reinterpret_cast<void*>( KERNEL_VIRTUAL_BASE + physical_addr );
	}

	return 0;
}

void*	mm::alloc_pages( unsigned int count, unsigned int align )
{
	uintptr_t physical_addr = physical::alloc( count, align );
	if( physical_addr )
	{
		return reinterpret_cast<void*>( KERNEL_VIRTUAL_BASE + physical_addr );
	}

	return 0;
}

uintptr_t	mm::alloc_pages_himem( unsigned int count, unsigned int align )
{
	return physical::alloc_himem( count, align );
}
