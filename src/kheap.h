
#ifndef _KHEAP_H_INC_
#define _KHEAP_H_INC_

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

inline void*	operator new( size_t size )
{
	return mm::heap::alloc( size );
}

inline void operator delete( void* ptr )
{
	return mm::heap::free( ptr );
}

#endif
