
#include "system.h"
#include "paging.h"

#include "memory.h"
#include "processor.h"
#include "debug.h"
#include "elf.h"
#include "console.h"

#ifndef _MSC_VER
#define ATTRIBUTE_PACKED__ __attribute__((packed))
#endif

struct ATTRIBUTE_PACKED__ page_dir_entry_t
{
	union
	{
		struct
		{
			unsigned int present			: 1;
			unsigned int writable			: 1;
			unsigned int user				: 1;
			unsigned int page_write_through	: 1;
			unsigned int page_cache_disable	: 1;
			unsigned int accessed			: 1;
			unsigned int dirty				: 1;	
			unsigned int enable4m			: 1;
			unsigned int reserved1			: 1;
			unsigned int available_bits		: 3;
			unsigned int reserved0			: 10;
			unsigned int page_frame_addr	: 10;
		};
		struct
		{
			unsigned int dummy				: 12;
			unsigned int page_table_addr	: 20;
		};
	};
};

/*
struct page_dir_entry_4M_t
{
	unsigned int present			: 1;
	unsigned int writable			: 1;
	unsigned int user				: 1;
	unsigned int page_write_through	: 1;
	unsigned int page_cache_disable	: 1;
	unsigned int accessed			: 1;
	unsigned int dirty				: 1;	
	unsigned int enable4m			: 1;
	unsigned int reserved1			: 1;
	unsigned int available_bits		: 3;
	unsigned int reserved0			: 10;
	unsigned int page_frame_addr	: 10;
};

struct page_dir_entry_4k_t
{
	unsigned int present			: 1;
	unsigned int writable			: 1;
	unsigned int user				: 1;
	unsigned int page_write_through	: 1;
	unsigned int page_cache_disable	: 1;
	unsigned int accessed			: 1;
	unsigned int dirty				: 1;	
	unsigned int enable4m			: 1;
	unsigned int reserved1			: 1;
	unsigned int available_bits		: 3;	
	unsigned int page_table_addr	: 20;
};
*/

struct page_table_entry_t
{
	unsigned int present			: 1;
	unsigned int writable			: 1;
	unsigned int user				: 1;
	unsigned int page_write_through	: 1;
	unsigned int page_cache_disable	: 1;
	unsigned int accessed			: 1;
	unsigned int dirty				: 1;
	unsigned int reserved1			: 2;
	unsigned int available_bits		: 3;	
	unsigned int page_addr			: 20;
};


extern "C" struct page_dir_entry_t* get_boot_pagedir( void );

struct page_dir_entry_t* g_kernel_page_directory = 0;


addr_t physical_to_virtual( struct page_dir_entry_t* pagedir, addr_t addr )
{
	TRACE2( pagedir, addr );

	struct page_dir_entry_t* pde = pagedir;
	for( int i = 0; i < 1024; ++i )
	{
		if( pde->present )
		{
			if( pde->enable4m )
			{
				if( pde->page_frame_addr == (addr >> 22) )
				{
					debug_bochs_printf( "ret = %d\n",  (i << 22) | (addr & 0x3FFFFF) );
					return (i << 22) | (addr & 0x3FFFFF);
				}
			}
			else
			{
				page_table_entry_t* pte = (page_table_entry_t*)(0xFFC00000 | (i << 12));

				for( int j = 0; j < 1024; ++j )
				{
					if( pte->present )
					{
						if( pte->page_addr == (addr >> 12) )
						{
							debug_bochs_printf( "ret = %d\n", (i << 22) | (j << 12) | (addr & 0xFFF) );
							return (i << 22) | (j << 12) | (addr & 0xFFF);
						}
					}

					pte++;
				}
			}
		}

		pde++;
	}
	
	debug_bochs_printf( "physical_to_virtual FAILED! (%x not mapped)\n", addr );
	return 0;
}

addr_t virtual_to_physical( struct page_dir_entry_t* pagedir, addr_t virtualaddr )
{
	//TRACE2( pagedir, virtualaddr );

    unsigned long pdindex = (unsigned long)virtualaddr >> 22;
    unsigned long ptindex = (unsigned long)virtualaddr >> 12 & 0x03FF;

    struct page_dir_entry_t* pd = pagedir;
	struct page_dir_entry_t* pde = pd + pdindex;

	if( pde->present )
	{
		if( pde->enable4m )
		{
			addr_t physical = (addr_t)(pde->page_frame_addr << 22) | (ptindex << 12) | (virtualaddr & 0xFFF);

			//debug_bochs_printf( "virtual_to_physical( %x ) = %x [4M]\n", virtualaddr, physical );
			return physical;
		}
		else
		{
			page_table_entry_t* pt = (page_table_entry_t*)(0xFFC00000 | (pdindex << 12));
			page_table_entry_t* pte = pt + ptindex;

			if( pte->present )
			{
				addr_t physical = (addr_t)((pte->page_addr << 12) + (virtualaddr & 0xFFF));

				//debug_bochs_printf( "virtual_to_physical( %x ) = %x [4k]\n", virtualaddr, physical );

				return physical;
			}
			else
			{
				debug_bochs_printf( "virtual_to_physical FAILED (%x)! PTE not present\n", virtualaddr );
				return 0;
			}
		}
	}
	else
	{
		debug_bochs_printf( "virtual_to_physical FAILED (%x)! PDE not present\n", virtualaddr );
		return 0;
	}    
}

/*
addr_t virtual_to_physical( addr_t addr )
{
	//return addr;

	// 4M pages case:
	page_dir_entry_4M_t* page_dir = get_boot_pagedir();
	page_dir_entry_4M_t* pde4M = &page_dir[addr >> 22];

	if( pde4M->enable4m )
	{
		return (pde4M->page_frame_addr << 22) | (addr & 0x3FFFFF);
	}
	else
	{
		debug_bochs_printf( "virtual_to_physical FAILED! (%x)\n", addr );
		return 0;
	}
}
*/

void invlpg( addr_t virtual_addr )
{
	asm( "invlpg (%%eax)" : : "a"(virtual_addr) );
}

addr_t myread_cr3()
{
	TRACE();

	addr_t dummy;
	asm( "mov %%cr3, %0" : "=r" (dummy));
	return dummy;
}


struct page_table_entry_t __attribute__((aligned(4096))) test_page_table[0x400];
struct page_table_entry_t __attribute__((aligned(4096))) kernel_page_table[0x400];

void paging_init( void )
{
	TRACE();

	debug_bochs_printf( "sizeof(page_dir_entry_t) = %x\n", sizeof(page_dir_entry_t) );

	debug_bochs_printf( "get_boot_pagedir() = 0x%x, myread_cr3() = 0x%x", get_boot_pagedir(), myread_cr3() );

	// Map the page directory to the last page directory entry, to allow access
	// to the page tables through virtual addresses
	struct page_dir_entry_t* page_dir = (page_dir_entry_t*)get_boot_pagedir();
	page_dir[0x3FF].present = 1;
	page_dir[0x3FF].writable = 1;
	page_dir[0x3FF].enable4m = 0;
	page_dir[0x3FF].page_table_addr = myread_cr3() >> 12;

	g_kernel_page_directory = (page_dir_entry_t*)0xFFFFF000;
}

extern CONSOLE console;

void paging_build_kernel_table( elf_section_header_table_t* esht, addr_t kernel_base )
{
	TRACE2( esht, kernel_base );

	memset( kernel_page_table, 0, sizeof(kernel_page_table) );	

	console.printf( "mbt->u.elf_sec.addr = %x\n", esht->addr );
	console.printf( "mbt->u.elf_sec.num = %x\n", esht->num );
	console.printf( "mbt->u.elf_sec.shndx = %x\n", esht->shndx );
	console.printf( "mbt->u.elf_sec.size = %x\n", esht->size );

	Elf32_Shdr* esh = (Elf32_Shdr*)esht->addr;

	for( size_t i = 0; i < esht->num; ++i )
	{
		if( esh->sh_flags & SHF_ALLOC )
		{
			console.printf( "ELF section: addr=%x size=%x writable=%s executable=%s\n",
				esh->sh_addr, esh->sh_size, esh->sh_flags & SHF_WRITE ? "yes" : "no",
				esh->sh_flags & SHF_EXECINSTR ? "yes" : "no" );

			size_t size = esh->sh_size;
			int pgindex = (esh->sh_addr >> 12) & 0x3FF;
			addr_t virtual_addr = esh->sh_addr;

			while( size > 0 )
			{
				page_table_entry_t* pte = &kernel_page_table[pgindex];
				pte->page_addr = virtual_to_physical( virtual_addr ) >> 12;
				pte->present = 1;
				pte->user = 0;
				pte->writable = esh->sh_flags & SHF_WRITE ? 1 : 0;

				debug_bochs_printf( "Mapping %x => %x (pgindex = %x)\n", virtual_addr, pte->page_addr << 12, pgindex );

				pgindex++;
				virtual_addr += 0x1000;
				if( size > 0x1000 )
					size -= 0x1000;
				else
					size = 0;
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
	pde.page_table_addr = virtual_to_physical( (addr_t)kernel_page_table ) >> 12;

	g_kernel_page_directory[kernel_base >> 22] = pde;

	debug_bochs_printf( "mega-invlpg...\n" );
	for( int i = 0; i < 0x400; ++i )
	{
		invlpg( kernel_base + i * 0x1000 );
	}
}

void map_page( addr_t physical_addr, addr_t virtual_addr, int flags )
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
    
    unsigned long pdindex = (unsigned long)virtual_addr >> 22;
    unsigned long ptindex = (unsigned long)virtual_addr >> 12 & 0x03FF;
    
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

	page_dir_entry_t* page_dir = get_boot_pagedir();

	addr_t virtual_addr = 0xB03FA000;
	addr_t physical_addr = 0x00400000;

	page_dir_entry_t pde;
	pde.writable = 1;
	pde.present = 1;
	pde.enable4m = 0;
	debug_bochs_printf( "page table @ virt %x\n", test_page_table );
	addr_t vtp = virtual_to_physical( (addr_t)test_page_table );
	debug_bochs_printf( "page table @ phys %x\n", vtp );
	pde.page_table_addr = virtual_to_physical( (addr_t)test_page_table ) >> 12;
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
	page_dir[virtual_addr >> 22] = pde;

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

extern "C" void paging_handle_fault( struct regs* r, addr_t virtual_address )
{
	debug_bochs_printf( "error code = %x, v_addr = %x\n", r->err_code, virtual_address );

	switch( r->err_code )
	{
	case 0: // KERNEL, READ, NOT_PRESENT
		r->err_code = 0;
		break;
	case 2: // KERNEL, WRITE, NOT_PRESENT:
		debug_bochs_printf( "Kernel tried to write to non-present page @ %x\n", virtual_address );
		break;
	default:
		debug_bochs_printf( "paging_handle_fault: no idea what to do!\n" );
		break;
	}
}
