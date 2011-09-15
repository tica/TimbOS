
#include "system.h"
#include "kheap.h"

#include "debug.h"
#include "paging.h"

const uintptr_t	KERNEL_HEAP_VBASE		= 0xE0000000;
const size_t	KERNEL_HEAP_SIZE		= 0x01000000;

const size_t	KERNEL_HEAP_PAGES		= KERNEL_HEAP_SIZE / 0x1000;
const size_t	KERNEL_HEAP_PAGETABLES	= KERNEL_HEAP_PAGES / 0x400;

static PageTable __ATTRIBUTE_PAGEALIGN__ s_kheap_page_tables[KERNEL_HEAP_PAGETABLES];

void kheap::init()
{
	debug_bochs_printf( "Heap size: %d pages, requiring %d page tables\n", KERNEL_HEAP_PAGES, KERNEL_HEAP_PAGETABLES );	

	for( unsigned int i = 0; i < KERNEL_HEAP_PAGETABLES; ++i )
	{
		PageTable& pt = s_kheap_page_tables[i];

		for( unsigned int j = 0; j < 0x400; ++j )
		{
			page_table_entry_t& pte = pt.entries[j];
			pte.present = 0;
			pte.writable = 1;
			pte.reserved_flag = 1;

			//debug_bochs_printf( "pte @ %x phy set as reserved\n", KernelPageDirectory.virtual_to_physical( &pte ) );
		}

		uintptr_t pt_phys = KernelPageDirectory.virtual_to_physical( s_kheap_page_tables + i );
		uintptr_t pt_mapped_virtual = KERNEL_HEAP_VBASE + i * 0x1000 * 0x400;

		unsigned int pd_index = pt_mapped_virtual >> 22;

		debug_bochs_printf( "Kernel Heap page table #%d pd_index %x\n", i, pd_index );

		auto& pd_entry = KernelPageDirectory[pd_index];
		pd_entry.present = 1;
		pd_entry.writable = 1;
		pd_entry.page_table_addr = pt_phys >> 12;
	}
}

uintptr_t kheap::alloc( size_t size )
{
	return size;
}
