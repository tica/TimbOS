
#include "FAT12DirectoryEnumBase.h"
#include "FAT12Directory.h"
#include "FAT12File.h"

#include "debug.h"

#include <optional>
#include <array>

namespace
{
	void extract_fn_part( char*& src, char*& dest, int count )
	{
		for( int i = 0; i < count; ++i )
		{
			char ch = *src++;
			if( ch != ' ' )
				* dest++ = ch;
		}
	}

	void extract_file_name( char fn[11], char* dest )
	{
		extract_fn_part( fn, dest, 8 );
		*dest++ = '.';
		extract_fn_part( fn, dest, 3 );

		if( *(dest - 1) == '.' )
			* (dest - 1) = 0;
	}
}

bool fs::fat12::FAT12DirectoryEnumBase::next( std::shared_ptr<fs::IFileSystemEntry>& file )
{
	std::optional<unsigned int> sector_cache_index;
	std::array<lowlevel::FAT12_DirectoryEntry, 512 / sizeof( lowlevel::FAT12_DirectoryEntry )> sector_cache;

	auto sectors = get_sectors();
	auto entry_count = get_entry_count();

	debug_bochs_printf( "sectors.size() = %d\n", sectors.size() );
	debug_bochs_printf( "entry_count = %d\n", entry_count );
	debug_bochs_printf( "_index = %d\n", _index );

	while( _index < entry_count )
	{
		unsigned int sector_index = _index / 16;
		unsigned int entry_index = _index % 16;

		if( sector_cache_index != sector_index )
		{
			_disk.read( sectors[sector_index], 1, &sector_cache );
			sector_cache_index = sector_index;
		}

		lowlevel::FAT12_DirectoryEntry& entry = sector_cache[entry_index];

		switch( entry.Name[0] )
		{
		case '\x000': // empty from here
			_index = entry_count;
			return false;
		case '\x00E': // empty
		case '\x0E5': // deleted
			_index += 1;
			continue;
		}

		char entry_name[13] = {};
		extract_file_name( entry.Name, entry_name );

		if( entry.Attribute & lowlevel::ATTR_DIRECTORY )
		{
			debug_bochs_printf( "DIRECTORY: %s (%d bytes, from cluster %d)\n", entry_name, entry.FileSize, entry.FileSystemClusterLogical );

			file = std::make_shared<FAT12Directory>( _parent, entry_name, (uint16_t)entry.FileSystemClusterLogical );
		}
		else
		{
			debug_bochs_printf( "FILE: %s (%d bytes, from cluster %d)\n", entry_name, entry.FileSize, entry.FileSystemClusterLogical );

			file = std::make_shared<FAT12File>( entry_name, (uint32_t)entry.FileSize, (uint16_t)entry.FileSystemClusterLogical );
		}
		debug_bochs_printf( "entry.Name[0] = 0x%2x\n", entry.Name[0] );
		debug_bochs_printf( "entry_name = %s\n", entry_name );
		debug_bochs_printf( "entry.Attribute = %2x\n", entry.Attribute );
		debug_bochs_printf( "entry.FileSize = %d\n", entry.FileSize );
		debug_bochs_printf( "entry.FileSystemClusterLogical = %d\n", entry.FileSystemClusterLogical );

		_index += 1;

		return true;
	}

	return false;
}
