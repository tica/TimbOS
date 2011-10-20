
#ifndef _BLOCKDEVICE_H_INC_
#define _BLOCKDEVICE_H_INC_

#include "driverbase.h"

namespace drv
{
	namespace itf
	{
		struct IBlockDevice
			:	public IDriverBase
		{
			virtual unsigned int	block_size() = 0;
			virtual unsigned int	block_count() = 0;

			virtual bool	read( unsigned int first_block_index, unsigned int block_count, void* buffer ) = 0;
			virtual bool	write( unsigned int first_block_index, unsigned int block_count, const void* buffer ) = 0;
		};
	}
}

#endif // _BLOCKDEVICE_H_INC_
