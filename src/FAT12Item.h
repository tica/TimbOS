#pragma once

#include "fs.h"
#include "FAT12.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <memory>

namespace fs
{
	namespace fat12
	{

		class FAT12Item
			: public fs::IFileSystemEntry
		{
		private:
			std::string _name;

		protected:
			cluster_id _firstCluster;

		protected:
			FAT12Item( std::string_view name, cluster_id firstCluster )
				: _name{ name }
				, _firstCluster{ firstCluster }
			{
			}

		public:
			virtual std::string_view name() override
			{
				return _name;
			}
		};

	}
}
