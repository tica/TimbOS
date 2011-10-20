
#include "system.h"

struct multiboot_info;

namespace mm
{
	void	init( const multiboot_info* mbt_info );

	// These functions return kernel-accessible virtual addresses
	void*	alloc_pages_dma( unsigned int count, unsigned int align = 1 );
	void*	alloc_pages( unsigned int count = 1, unsigned int align = 1 );

	// addr is a virtual address that was previously returned from alloc_pages or alloc_pages_dma
	void	free_pages( void* addr, unsigned int count = 1 );

	// Get the physical address for a pointer into a page obtained from alloc_pages or alloc_pages_dma
	uintptr_t	kernel_virtual_to_physical( void* p );

	// Get the kernel-mapped virtual address for a physical address in the range of kernel or dma pages
	void*		kernel_physical_to_virtual( uintptr_t p );

	// This function returns physical addresses!
	uintptr_t	alloc_pages_himem( unsigned int count = 1, unsigned int align = 1 );
}
