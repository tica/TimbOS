
#ifndef _PHYSMEM_H_
#define _PHYSMEM_H_

#include "system.h"
#include "multiboot.h"

addr_t physmem_alloc_pageframe();
void physmem_free_pageframe( addr_t physical_addr );
void physmem_init_from_mbt( multiboot_info_t* mbt );

#endif // _PHYSMEM_H_
