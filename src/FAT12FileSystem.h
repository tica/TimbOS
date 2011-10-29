
#ifndef _FAT12FILESYSTEM_H_INC_
#define _FAT12FILESYSTEM_H_INC_

#include "fs.h"
#include "diskcache.h"

namespace fs
{
	struct FAT12_BootSector;

	class FAT12FileSystem
		:	public IFileSystem
	{
	private:
		drv::itf::IDiskCache*		_pDisk;

		drv::itf::IDiskCacheBuffer*	_pBootSectorBuffer;
		drv::itf::IDiskCacheBuffer*	_pFATBuffer;
		drv::itf::IDiskCacheBuffer*	_pRootBuffer;

		FAT12_BootSector*			_pBootSector;

	private:
		FAT12FileSystem( drv::itf::IDiskCache* pDisk, drv::itf::IDiskCacheBuffer* pBootSectorBuffer, FAT12_BootSector* pBootSector );

	public:
		virtual ~FAT12FileSystem();

		static FAT12FileSystem* tryCreate( drv::itf::IDiskCache* pDisk, drv::itf::IDiskCacheBuffer* pBootSectorBuffer );

	public:
		virtual IDirectoryEnum* root_directory();
	};
}

#endif // _FAT12FILESYSTEM_H_INC_
