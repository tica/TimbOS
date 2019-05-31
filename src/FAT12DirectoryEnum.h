#pragma once

#include "FAT12FileSystem.h"
#include "FAT12DirectoryEnumBase.h"

#include "debug.h"

#include <vector>
#include <cstdint>
#include <memory>

namespace fs
{
	namespace fat12
	{
		class FAT12DirectoryEnum : public FAT12DirectoryEnumBase
		{
			std::vector<uint16_t>	_sectors;

		public:
			FAT12DirectoryEnum( std::shared_ptr<FAT12FileSystem> parent, unsigned int first_cluster )
				: FAT12DirectoryEnumBase( parent )
			{
				unsigned int sectors_per_cluster = _parent->sectors_per_cluster();
				unsigned int cluster = first_cluster;

				do
				{
					unsigned int sector = _parent->sector_from_cluster( cluster );

					debug_bochs_printf( "FAT12DirectoryEnum: add cluster at sector %d", sector );

					for( unsigned int i = 0; i < sectors_per_cluster; ++i )
					{
						_sectors.push_back( sector + i );
					}

					cluster = _parent->next_cluster( cluster );
				} while( cluster != 0xFF0 && cluster != 0xFFF );
			}

		protected:
			virtual std::vector<uint16_t> get_sectors() override
			{
				return _sectors;
			}

			virtual size_t get_entry_count() override
			{
				return _sectors.size() * 16;
			}
		};
	}
}
