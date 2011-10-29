
#ifndef _DISKCACHE_H_INC_
#define _DISKCACHE_H_INC_

#include "driverbase.h"

namespace drv
{
	namespace itf
	{
		struct IDiskCacheBuffer
		{
			virtual ~IDiskCacheBuffer() {}

			virtual	size_t	length() = 0;
			virtual size_t	block_count() = 0;

			virtual void*	lock() = 0;
			virtual void	unlock() = 0;

			virtual bool	locked() = 0;
			virtual bool	free() = 0;
		};

		struct IDiskCache
			:	public IDriverBase
		{
			virtual unsigned int		device_block_count() = 0;
			virtual unsigned int		device_block_size() = 0;

			virtual IDiskCacheBuffer*	prepare( unsigned int first_block, unsigned int block_count ) = 0;
			virtual IDiskCacheBuffer*	cache( unsigned int first_block, unsigned int block_count ) = 0;
		};
	}
}

#endif // _DISKCACHE_H_INC_
