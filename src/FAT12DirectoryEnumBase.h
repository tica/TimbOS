#pragma once

#include "FAT12FileSystem.h"
#include "FAT12LowLevel.h"
#include "FAT12Directory.h"
#include "FAT12File.h"

#include "fs.h"

#include <memory>

namespace fs
{
	namespace fat12
	{
		class FAT12DirectoryEnumBase
			: public fs::IDirectoryEnum
		{
		protected:
			std::shared_ptr<FAT12FileSystem> _parent;

		private:
			drv::itf::IBlockDevice& _disk;

			unsigned int			_index = 0;

		protected:
			FAT12DirectoryEnumBase( std::shared_ptr<FAT12FileSystem> parent )
				: _parent{ parent }
				, _disk{ parent->dev() }
			{
			}

			virtual std::vector<uint16_t> get_sectors() = 0;
			virtual size_t get_entry_count() = 0;

		public:
			virtual void reset() override
			{
				_index = 0;
			}

			virtual bool next( std::shared_ptr<fs::IFileSystemEntry>& file ) override;
		};
	}
}