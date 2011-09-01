
#ifndef _PAGING_H_
#define _PAGING_H_

#include "system.h"
#include "multiboot.h"

struct page_dir_entry_t;

#ifdef __cplusplus
extern "C"
{
#endif
	extern struct page_dir_entry_t* g_kernel_page_directory;

	addr_t physical_to_virtual( struct page_dir_entry_t* pagedir, addr_t addr );
	addr_t virtual_to_physical( struct page_dir_entry_t* pagedir, addr_t addr );

	void paging_init( void );
	void paging_build_kernel_table( elf_section_header_table_t* esht, addr_t kernel_base );

	void map_page( addr_t physical_addr, addr_t virtual_addr );

	void paging_test( void );
#ifdef __cplusplus
}

template<class T>
T physical_to_virtual( struct page_dir_entry_t* pagedir, T addr )
{
	return (T)physical_to_virtual( pagedir, (addr_t)addr );
}

template<class T>
T virtual_to_physical( struct page_dir_entry_t* pagedir, T addr )
{
	return (T)virtual_to_physical( pagedir, (addr_t)addr );
}

template<class T>
T physical_to_virtual( T addr )
{
	return (T)physical_to_virtual( g_kernel_page_directory, (addr_t)addr );
}

template<class T>
T virtual_to_physical( T addr )
{
	return (T)virtual_to_physical( g_kernel_page_directory, (addr_t)addr );
}
#endif

#endif // _PAGING_H_
