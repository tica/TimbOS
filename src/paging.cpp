
#include "system.h"
#include "paging.h"

#include "memory.h"
#include "processor.h"
#include "debug.h"
#include "elf.h"
#include "console.h"
#include "pmem.h"


void invlpg( uintptr_t virtual_addr )
{
	asm( "invlpg (%%eax)" : : "a"(virtual_addr) );
}

/*
uintptr_t read_cr3()
{
	uintptr_t dummy;
	asm( "mov %%cr3, %0" : "=r" (dummy));
	return dummy;
}
*/


struct page_table_entry_t __ATTRIBUTE_PAGEALIGN__ test_page_table[0x400];
struct page_table_entry_t __ATTRIBUTE_PAGEALIGN__ kernel_page_table[0x400];

struct page_table_entry_t __ATTRIBUTE_PAGEALIGN__ kernel_heap_page_tables[0x400][0x10];

void paging_init( void )
{
	TRACE();

	debug_bochs_printf( "KernelPageDirectory = %x, cr3 = 0x%x\n", &KernelPageDirectory, read_cr3() );

	KernelPageDirectory.init();
}

extern CONSOLE console;

void paging_build_kernel_table( elf_section_header_table_t* esht, uintptr_t kernel_base )
{
	TRACE2( esht, kernel_base );

	memset( kernel_page_table, 0, sizeof(kernel_page_table) );	

	debug_bochs_printf( "mbt->u.elf_sec.addr = %x\n", esht->addr );
	debug_bochs_printf( "mbt->u.elf_sec.num = %x\n", esht->num );
	debug_bochs_printf( "mbt->u.elf_sec.shndx = %x\n", esht->shndx );
	debug_bochs_printf( "mbt->u.elf_sec.size = %x\n", esht->size );

	Elf32_Shdr* esh = (Elf32_Shdr*)esht->addr;

	for( size_t i = 0; i < esht->num; ++i )
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

				page_table_entry_t* pte = &kernel_page_table[pgindex];
				pte->page_addr = physical_address >> 12;
				pte->present = 1;
				pte->user = 0;
				pte->writable = esh->sh_flags & SHF_WRITE ? 1 : 0;

				//debug_bochs_printf( "Mapping %x => %x (pgindex = %x)\n", virtual_addr, physical_address, pgindex );

				pgindex++;

				unsigned int page_step = 0x1000 - (virtual_addr % 0x1000);
				if( page_step > size ) page_step = size;

				virtual_addr += page_step;
				size -= page_step;
			}
		}
		esh++;
	}

	// Map video memory
	page_table_entry_t* pte = &kernel_page_table[0x0B8];
	pte->writable = 1;
	pte->present = 1;
	pte->page_addr = 0x0B8;

	// Map multiboot hdr
	pte = &kernel_page_table[0x02B];
	pte->writable = 1;
	pte->present = 1;
	pte->page_addr = 0x02B;

	struct page_dir_entry_t pde;
	pde.writable = 1;
	pde.present = 1;
	pde.enable4m = 0;
	pde.page_table_addr = (uintptr_t)KernelPageDirectory.virtual_to_physical( kernel_page_table ) >> 12;

	KernelPageDirectory[kernel_base >> 22] = pde;

	debug_bochs_printf( "mega-invlpg...\n" );
	for( int i = 0; i < 0x400; ++i )
	{
		invlpg( kernel_base + i * 0x1000 );
	}
}

void map_page( uintptr_t physical_addr, uintptr_t virtual_addr, int flags )
{
	TRACE3( physical_addr, virtual_addr, flags );

    if( physical_addr & 0xFFF )
	{
		debug_bochs_printf( "map_page: physical_addr not page-aligned (%x)\n", physical_addr );
		return;
	}
	if( virtual_addr & 0xFFF )
	{
		debug_bochs_printf( "map_page: physical_addr not page-aligned (%x)\n", virtual_addr );
		return;
	}
    
    unsigned long pdindex = virtual_addr >> 22;
    unsigned long ptindex = virtual_addr >> 12 & 0x03FF;
    
    //unsigned long * pd = (unsigned long *)0xFFFFF000;
	//pd = 0;
    // Here you need to check whether the PD entry is present.
    // When it is not present, you need to create a new empty PT and
    // adjust the PDE accordingly.
    
    unsigned long * pt = ((unsigned long *)0xFFC00000) + (0x400 * pdindex);
    // Here you need to check whether the PT entry is present.
    // When it is, then there is already a mapping present. What do you do now?
    
    pt[ptindex] = ((unsigned long)physical_addr) | (flags & 0xFFF) | 0x01; // Present
    
    // Now you need to flush the entry in the TLB
    // or you might not notice the change.

}

void paging_test( void )
{
	TRACE();

	uintptr_t vtest = KernelPageDirectory.virtual_to_physical( 0xFFF00000 );
	debug_bochs_printf( "0xFFF00000 @ phys %x\n", vtest );

	uintptr_t virtual_addr = 0xB03FA000;
	uintptr_t physical_addr = 0x00400000;

	page_dir_entry_t pde;
	pde.writable = 1;
	pde.present = 1;
	pde.enable4m = 0;
	debug_bochs_printf( "page table @ virt %x\n", test_page_table );
	uintptr_t vtp = KernelPageDirectory.virtual_to_physical( test_page_table );
	debug_bochs_printf( "page table @ phys %x\n", vtp );
	pde.page_table_addr = KernelPageDirectory.virtual_to_physical( test_page_table ) >> 12;
	debug_bochs_printf( "pde.page_table_addr = %x\n", pde.page_table_addr );

	page_table_entry_t pte;
	pte.present = 1;
	pte.writable = 1;
	pte.page_addr = (physical_addr >> 12);
	
	debug_bochs_printf( "clear page table... " );
	memset( test_page_table, 0, sizeof(test_page_table) );
	debug_bochs_printf( "done\n" );

	test_page_table[(virtual_addr >> 12) & 0x3FF] = pte;
	test_page_table[((virtual_addr >> 12) & 0x3FF) + 1] = pte;
	KernelPageDirectory[virtual_addr >> 22] = pde;

	invlpg( virtual_addr );
	
	debug_bochs_printf( "done\n" );

	//set_pagedir();

	int* p = (int*)0xB03FA000;
	int* q = (int*)0xB03FB000;

	debug_bochs_printf( "*p = %x\n", *p );
	*p = 0x12345678;
	debug_bochs_printf( "*p = %x\n", *p );
	debug_bochs_printf( "*q = %x\n", *q );	
}

extern "C" bool paging_handle_fault( struct cpu_state* r, uintptr_t virtual_address )
{
	bool protection_violation = r->err_code & 0x1;
	//bool access_write = r->err_code & 0x2;
	//bool user_mode = r->err_code & 0x4;
	bool reserved_violation = r->err_code & 0x8;

	if( reserved_violation )
	{
		debug_bochs_printf( "page fault @ %x: reserved_violation!\n", virtual_address );
	}
	else if( protection_violation )
	{
		debug_bochs_printf( "page fault @ %x: protection_violation!\n", virtual_address );
	}
	else // non-present page
	{
		unsigned int pd_index = virtual_address >> 22;
		unsigned int pt_index = (virtual_address >> 12) & 0x3FF;

		auto& pde = KernelPageDirectory[pd_index];
		if( pde.present )
		{
			//debug_bochs_printf( "Page directory entry %x is present...\n", pd_index );

			if( !pde.enable4m )
			{
				page_table_entry_t* pte = (page_table_entry_t*)(0xFFC00000 | (pd_index << 12) | (pt_index << 2));

				//debug_bochs_printf( "checking pte @ %x phy %x virt...\n", KernelPageDirectory.virtual_to_physical( pte ), pte );
				
				if( pte->reserved_flag ) // Memory has been 'reserved' => auto-alloc physical
				{
					uintptr_t phys_addr = pmem_alloc();
					debug_bochs_printf( "paging_handle_fault: page was reserved, allocated new phys page @ %x\n", phys_addr );

					pte->page_addr = phys_addr >> 12;					
					pte->reserved_flag = 0;
					pte->present = 1;

					//debug_bochs_printf( "*pte = %x\n", *pte );

					return true;
				}
				else
				{
					debug_bochs_printf( "page fault @ %x: page not present!\n", virtual_address );
				}
			}
			else
			{
				debug_bochs_printf( "page fault @ %x: 4M pages not supported!\n", virtual_address );
			}
		}
		else
		{
			debug_bochs_printf( "page fault @ %x: Page directory entry 0x%x is NOT present...\n", virtual_address, pd_index );
		}
	}

	return false;
}
