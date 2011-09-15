
#ifndef _PAGE_DIRECTORY_H_INC_
#define _PAGE_DIRECTORY_H_INC_

#include "system.h"

struct __ATTRIBUTE_PACKED__ page_dir_entry_t
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
	unsigned int reserved_flag		: 1; // first free bit
	unsigned int available_bits		: 2; // two more free bits
	unsigned int page_addr			: 20;
};

struct PageDirectory
{
	struct page_dir_entry_t	entries[1024];

	PageDirectory();
	void init();

	page_dir_entry_t& operator[]( int idx )
	{
		return entries[idx];
	}

	uintptr_t virtual_to_physical( uintptr_t addr ) const;
	uintptr_t virtual_to_physical( void* addr ) const
	{
		return virtual_to_physical( (uintptr_t)addr );
	}

	uintptr_t physical_to_virtual( uintptr_t addr ) const;
	
	void map_page( uintptr_t physical_addr, uintptr_t virtual_addr, int flags );
};

struct PageTable
{
	struct page_table_entry_t entries[1024];

	page_table_entry_t& operator[]( int idx )
	{
		return entries[idx];
	}
};



#endif // _PAGE_DIRECTORY_H_INC_