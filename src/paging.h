
#ifndef _PAGING_H_
#define _PAGING_H_

#include "system.h"
#include "multiboot.h"

#include "pagedir.h"

extern PageDirectory KernelPageDirectory; // coming from loader.s

void paging_init( void );
void paging_build_kernel_table( elf_section_header_table_t* esht, uintptr_t kernel_base );

void map_page( uintptr_t physical_addr, uintptr_t virtual_addr );

void paging_test( void );

#endif // _PAGING_H_
