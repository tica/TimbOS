#pragma once

#include "FAT12Item.h"

#include "fs.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <memory>

namespace fs
{
	namespace fat12
	{

		class FAT12File
			: public FAT12Item
		{
			uint32_t _size;

		public:
			FAT12File( std::string_view name, uint32_t fileSize, uint16_t firstCluster )
				: FAT12Item( name, firstCluster )
				, _size( fileSize )
			{
			}

			virtual std::shared_ptr<fs::IDirectoryEnum> children() override
			{
				return nullptr;
			}

			uint32_t size() const
			{
				return _size;
			}
		};

	}
}
