
#ifndef _PHYSMEM_H_
#define _PHYSMEM_H_

#include "system.h"

struct multiboot_info;


namespace mm
{
	namespace physical
	{
		enum PhysicalArea
		{
			PHYAREA_DMA = 0,
			PHYAREA_DEFAULT = 1,
			PHYAREA_HIMEM = 2
		};

		void		init( const multiboot_info* mbt, uintptr_t kernel_static_end, uintptr_t kernel_dynamic_end, uintptr_t himem_end );
		
		uintptr_t	alloc_dma( unsigned int pages = 1, unsigned int align = 1 );
		uintptr_t	alloc( unsigned int pages = 1, unsigned int align = 1 );
		uintptr_t	alloc_himem( unsigned int pages = 1, unsigned int align = 1 );

		void		free( uintptr_t physical_addr, unsigned int pages );
	}
}

#endif // _PHYSMEM_H_
