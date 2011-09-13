
#ifndef _PHYSMEM_H_
#define _PHYSMEM_H_

#include "system.h"

struct multiboot_info;

void		pmem_init( struct multiboot_info* mbt );

uintptr_t	pmem_alloc();
void		pmem_free( uintptr_t physical_addr );

void		pmem_mark_used( uintptr_t physical_addr );

#endif // _PHYSMEM_H_
