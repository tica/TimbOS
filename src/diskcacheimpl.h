
#ifndef _DISKCACHEIMPL_H_INC_
#define _DISKCACHEIMPL_H_INC_

#include "diskcache.h"
#include "blockdevice.h"

#include <map>

class DiskCacheBuffer;

namespace drv
{
	class DiskCache
		:	public itf::IDiskCache
	{
		itf::IBlockDevice*	_pDevice;

		std::map<unsigned int, DiskCacheBuffer*>	_cache;

	public:
		DiskCache( itf::IBlockDevice* pDevice );

		virtual const char*	description();
		virtual void		init();

		virtual unsigned int				device_block_count();
		virtual unsigned int				device_block_size();

		virtual drv::itf::IDiskCacheBuffer*	prepare( unsigned int first_block, unsigned int block_count );
		virtual drv::itf::IDiskCacheBuffer*	cache( unsigned int first_block, unsigned int block_count );
	};
}

#endif // _DISKCACHEIMPL_H_INC_
