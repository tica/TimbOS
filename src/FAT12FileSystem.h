
#pragma once

#include "fs.h"
#include "blockdevice.h"

#include "FAT12FAT.h"
#include "FAT12LowLevel.h"

#include "system.h"

#include <map>
#include <memory>

namespace fs
{
	namespace fat12
	{
		class FAT12FileSystem
			: public IFileSystem
			, private std::enable_shared_from_this<FAT12FileSystem>
		{
			FAT12FileSystem( const FAT12FileSystem& other ) = delete;
			FAT12FileSystem( const FAT12FileSystem&& other ) = delete;

		protected:
			std::weak_ptr<FAT12FileSystem> _weak_this;

		private:
			drv::itf::IBlockDevice* _pDisk;

			lowlevel::FAT12_BootSector	_bootSector;
			size_t						_firstDataSector;

			FAT12FAT					_fat;

			std::map<uint64_t, std::shared_ptr<IFileSystemEntry>>	_openItems;

		public:
			unsigned int sector_from_cluster( cluster_id cluster );
			int sectors_per_cluster() { return _bootSector.SectorsPerCluster; }
			cluster_id next_cluster( cluster_id cluster ) { return _fat.next_cluster( cluster ); }

		public:
			FAT12FileSystem( drv::itf::IBlockDevice* pDisk, lowlevel::FAT12_BootSector* pBootSector );

		public:
			virtual ~FAT12FileSystem() override;

			static std::shared_ptr<FAT12FileSystem> tryCreate( drv::itf::IBlockDevice* pDisk, void* pBootSectorData );

		public:
			virtual std::shared_ptr<IDirectoryEnum> root_directory() override;

			drv::itf::IBlockDevice& dev()
			{
				return *_pDisk;
			}
		};
	}
}
