
#ifndef _PAGING_H_
#define _PAGING_H_

#include "system.h"
#include "multiboot.h"

#include "pagedir.h"

extern PageDirectory KernelPageDirectory; // coming from loader.s

namespace mm
{
	namespace paging
	{
		void init( uintptr_t linear_mapping_end, uintptr_t virtual_base );
		void paging_build_kernel_table( multiboot_elf_section_header_table* esht, uintptr_t kernel_base );

		void map_page( uintptr_t physical_addr, uintptr_t virtual_addr, int flags );

		void paging_test( void );
	}
}

#endif // _PAGING_H_
