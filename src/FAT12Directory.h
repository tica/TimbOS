#pragma once

#include "FAT12FileSystem.h"
#include "FAT12Item.h"

#include "fs.h"

#include <cstdint>
#include <string>
#include <string_view>

namespace fs
{
	namespace fat12
	{
		class FAT12Directory
			: public FAT12Item
		{
			std::shared_ptr<FAT12FileSystem> _fs;

		public:
			FAT12Directory( std::shared_ptr<FAT12FileSystem> fs, std::string_view name, uint16_t firstCluster )
				: FAT12Item( name, firstCluster )
				, _fs{ fs }
			{
			}

			virtual std::shared_ptr<fs::IDirectoryEnum> children() override;
		};
	}
}
