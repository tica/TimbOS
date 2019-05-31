#pragma once

#include "FAT12.h"

#include "debug.h"

#include <cstdint>
#include <map>
#include <vector>

namespace fs
{
	namespace fat12
	{

		class FAT12FAT
		{
			std::vector<cluster_id> fat_;

		public:
			cluster_id next_cluster( cluster_id cluster )
			{
				if( cluster > fat_.size() )
				{
					panic( "Access outside bounds of FAT: cluster = %d", cluster );
				}

				return fat_[cluster];
			}

			void read( const uint8_t* src, size_t length )
			{
				if( length % 3 != 0 )
				{
					panic( "Unexpected length of FAT: length = %d", length );
				}

				fat_.resize( length * 2 / 3 );

				for( size_t i = 0; i < length / 3; ++i )
				{
					int f0 = src[i * 3];
					int f1 = src[i * 3 + 1];
					int f2 = src[i * 3 + 2];

					cluster_id u = (f0) | ((f1 << 8) & 0xF00);
					cluster_id v = (f2 << 4) | ((f1 >> 4) & 0x00F);

					fat_[i * 2 + 0] = u;
					fat_[i * 2 + 1] = v;
				}

#if 0
				for( size_t i = 0; i < fat_.size(); ++i )
				{
					debug_bochs_printf( "%04X -> %04X ", i, fat_[i] );

					if( i % 4 == 0 )
						debug_bochs_printf( "\n" );

					if( fat_[i] == 0xFFF )
						debug_bochs_printf( "\n----------\n" );
				}
#endif		
			}

			void write( void* ptr, size_t space )
			{
				(void)ptr;
				(void)space;

				if( space < fat_.size() * 2 / 3 )
				{
					panic( "Not enough space for FAT: space = %d, fat size = %d", space, fat_.size() );
				}
				
				auto p = (uint8_t*)ptr;
				auto f = fat_.begin();

				while( f != fat_.end() )
				{
					cluster_id u = *f++;
					cluster_id v = *f++;

					*p++ = u & 0xFF;
					*p++ = ((u & 0x0F00) >> 8) | ((v & 0x00F) << 4);
					*p++ = (v & 0xFF0) >> 4;
				}
			}
		};

	}
}
