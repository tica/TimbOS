
#ifndef _PHYSMEM_H_
#define _PHYSMEM_H_

#include "system.h"

struct multiboot_info;

enum PhysicalArea
{
	PHYAREA_DEFAULT = 0,
	PHYAREA_DMA
};

void		pmem_init( struct multiboot_info* mbt );

uintptr_t	pmem_alloc_dma( unsigned int pages, unsigned int align );
uintptr_t	pmem_alloc();
uintptr_t	pmem_alloc( unsigned int pages, unsigned int align );

void		pmem_free( uintptr_t physical_addr );

void		pmem_reserve( uintptr_t physical_addr );

#endif // _PHYSMEM_H_
