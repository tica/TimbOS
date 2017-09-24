
#ifndef _KHEAP_H_INC_
#define _KHEAP_H_INC_

#include "debug.h"

namespace mm
{
	namespace heap
	{
		void	init();

		void*	alloc( size_t size );
		void	free( void* ptr );

		void	stat();
	}
}

void*	operator new(size_t size);
void*	operator new[](size_t size);
void operator delete(void* ptr);
void operator delete(void* ptr, long unsigned int);
void operator delete[](void* ptr);
void operator delete[](void* ptr, long unsigned int);

#endif
