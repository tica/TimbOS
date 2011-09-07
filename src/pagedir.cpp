
#include "system.h"
#include "pagedir.h"

#include "debug.h"

uintptr_t PageDirectory::virtual_to_physical( uintptr_t addr ) const
{
	uintptr_t virtualaddr = (uintptr_t)addr;

	//TRACE2( pagedir, virtualaddr );

    unsigned long pdindex = virtualaddr >> 22;
    unsigned long ptindex = virtualaddr >> 12 & 0x03FF;

	//debug_bochs_printf( "pdindex = 0x%x, ptindex = 0x%x\n", pdindex, ptindex );

    const struct page_dir_entry_t* pd = entries;
	const struct page_dir_entry_t* pde = pd + pdindex;

	//debug_bochs_printf( "pd = 0x%x, pde = 0x%x, pde->page_table_addr = 0x%x\n", pd, pde, pde->page_table_addr );

	if( pde->present )
	{
		if( pde->enable4m )
		{
			uintptr_t physical = (pde->page_frame_addr << 22) | (ptindex << 12) | (virtualaddr & 0xFFF);

			//debug_bochs_printf( "virtual_to_physical( %x ) = %x [4M]\n", virtualaddr, physical );
			return physical;
		}
		else
		{
			page_table_entry_t* pt = (page_table_entry_t*)(0xFFC00000 | (pdindex << 12));			
			page_table_entry_t* pte = pt + ptindex;

			//debug_bochs_printf( "pt = 0x%x, pte = 0x%x\n", pt, pte );

			if( pte->present )
			{
				uintptr_t physical = ((pte->page_addr << 12) + (virtualaddr & 0xFFF));

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

uintptr_t PageDirectory::physical_to_virtual( uintptr_t addr ) const
{
	TRACE2( entries, addr );

	const struct page_dir_entry_t* pde = entries;
	for( int i = 0; i < 1024; ++i )
	{
		if( pde->present )
		{
			if( pde->enable4m )
			{
				if( pde->page_frame_addr == (addr >> 22) )
				{
					debug_bochs_printf( "ret = %d\n",  (i << 22) | (addr & 0x3FFFFF) );
					return ((i << 22) | (addr & 0x3FFFFF));
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
							return ((i << 22) | (j << 12) | (addr & 0xFFF));
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