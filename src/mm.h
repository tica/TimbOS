
#include "system.h"

struct multiboot_info;

namespace mm
{
	void	init( const multiboot_info* mbt_info );

	// These functions return kernel-accessible virtual addresses
	void*	alloc_pages_dma( unsigned int count, unsigned int align = 1 );
	void*	alloc_pages( unsigned int count = 1, unsigned int align = 1 );

	// This function returns physical addresses!
	uintptr_t	alloc_pages_himem( unsigned int count = 1, unsigned int align = 1 );	
}
