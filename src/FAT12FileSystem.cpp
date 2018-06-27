
#include "system.h"
#include "FAT12FileSystem.h"
#include "debug.h"

#include <string.h>
#include <stdint.h>

#include <array>
#include <optional>

using namespace fs;

namespace fs
{
	namespace lowlevel
	{
		enum FAT12Attributes
		{
			ATTR_READ_ONLY = 0x01,
			ATTR_HIDDEN = 0x02,
			ATTR_SYSTEM = 0x04,
			ATTR_VOLUME_ID = 0x08,
			ATTR_DIRECTORY = 0x10,
			ATTR_ARCHIVE = 0x20,
		};

		struct __ATTRIBUTE_PACKED__ FAT12_DirectoryEntry
		{
			char		Name[11];
			uint8_t		Attribute;
			uint8_t		Reserved0;
			uint8_t		CreateTimeTenth;
			uint16_t	Reserved1;
			uint16_t	LastAccessDate;
			uint16_t	Reserved2;
			uint16_t	Reserved3;
			uint16_t	WriteTime;
			uint16_t	WriteDate;
			uint16_t	FileSystemClusterLogical;
			uint32_t	FileSize;
		};
	}
}

class FAT12Directory
	:	public IFileSystemEntry
{
	unsigned short _firstCluster;

public:
	FAT12Directory( unsigned short firstCluster )
		:	_firstCluster( firstCluster )
	{
	}
};

class FAT12File
	:	public IFileSystemEntry
{
	unsigned short _firstCluster;

public:
	FAT12File( unsigned short firstCluster )
		:	_firstCluster( firstCluster )
	{
	}
};

static void extract_fn_part( char*& src, char*& dest, int count )
{
	for( int i = 0; i < count; ++i )
	{
		char ch = *src++;
		if( ch != ' ' )
			*dest++ = ch;
	}
}

static void extract_file_name( char fn[11], char* dest )
{
	extract_fn_part( fn, dest, 8 );
	*dest++ = '.';
	extract_fn_part( fn, dest, 3 );

	if( *(dest-1) == '.' )
		*(dest-1) = 0;
}

class FAT12DirectoryEnum
	:	public IDirectoryEnum
{
	drv::itf::IBlockDevice*	_pDisk;
	unsigned int			_first_sector;
	unsigned int			_sector_count;

	unsigned int			_entry_count;
	unsigned int			_index;

public:
	FAT12DirectoryEnum( drv::itf::IBlockDevice* pDisk, unsigned int first_sector, unsigned int sector_count )
		:_pDisk{ pDisk }
		, _first_sector{ first_sector }
		, _sector_count{ sector_count }
		, _entry_count{ sector_count * 16 }
		, _index{ 0 }
	{
	}

	void reset()
	{
		_index = 0;
	}

	bool next( IFileSystemEntry** file )
	{
		(void)file;

		std::optional<unsigned int> sector_cache_index;
		std::array<lowlevel::FAT12_DirectoryEntry, 512 / sizeof(lowlevel::FAT12_DirectoryEntry)> sector_cache;

		while( _index < _entry_count )
		{
			unsigned int sector_index = _index / 16;
			unsigned int entry_index = _index % 16;

			if (sector_cache_index != sector_index)
			{
				_pDisk->read(_first_sector + sector_index, 1, &sector_cache);
				sector_cache_index = sector_index;
			}

			lowlevel::FAT12_DirectoryEntry& entry = sector_cache[entry_index];

			switch( entry.Name[0] )
			{
			case '\x000': // empty from here
				_index = _entry_count;
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
			}
			else
			{
				debug_bochs_printf( "FILE: %s (%d bytes, from cluster %d)\n", entry_name, entry.FileSize, entry.FileSystemClusterLogical );
			}
			debug_bochs_printf( "entry.Name[0] = 0x%2x\n", entry.Name[0] );
			debug_bochs_printf( "entry_name = %s\n", entry_name);
			debug_bochs_printf( "entry.Attribute = %2x\n", entry.Attribute );
			debug_bochs_printf( "entry.FileSize = %d\n", entry.FileSize );
			debug_bochs_printf( "entry.FileSystemClusterLogical = %d\n", entry.FileSystemClusterLogical );

			_index += 1;

			return true;
		}

		return false;
	}
};


FAT12FileSystem::FAT12FileSystem( drv::itf::IBlockDevice* pDisk, lowlevel::FAT12_BootSector* pBootSector )
	: _pDisk{ pDisk }
	, _bootSector{ *pBootSector }
{
	debug_bochs_printf( "Creating FAT12 File system\n" );
	debug_bochs_printf( "OSName = %s\n", _bootSector.OSName );
	debug_bochs_printf( "BytesPerSector = %d\n", _bootSector.BytesPerSector );
	debug_bochs_printf( "SectorsPerCluster = %d\n", _bootSector.SectorsPerCluster );
	debug_bochs_printf( "ReservedSectorCount = %d\n", _bootSector.ReservedSectorCount );
	debug_bochs_printf( "NumFATs = %d\n", _bootSector.NumFATs );
	debug_bochs_printf( "RootEntityCount = %d\n", _bootSector.RootEntityCount );
	debug_bochs_printf( "TotalSectorCount = %d\n", _bootSector.TotalSectorCount );
	debug_bochs_printf( "MediaType = %2x\n", _bootSector.MediaType );
	debug_bochs_printf( "FATSize = %d\n", _bootSector.FATSize );
	debug_bochs_printf( "SectorsPerTrack = %d\n", _bootSector.SectorsPerTrack );
	debug_bochs_printf( "NumHeads = %d\n", _bootSector.NumHeads );
	debug_bochs_printf( "HiddenSectors = %d\n", _bootSector.HiddenSectors );
	debug_bochs_printf( "TotalSectors32 = %d\n", _bootSector.TotalSectors32 );
	debug_bochs_printf( "DriveNumber = %d\n", _bootSector.DriveNumber );
	debug_bochs_printf( "VolumeID = %x\n", _bootSector.VolumeID );

	unsigned int firstFATsector = _bootSector.ReservedSectorCount;
	unsigned int firstRootDirectorySector = firstFATsector + _bootSector.NumFATs * _bootSector.FATSize;
	unsigned int numRootDirectorySectors = (_bootSector.RootEntityCount * sizeof(lowlevel::FAT12_DirectoryEntry)) / _bootSector.BytesPerSector;
	unsigned int firstDataSector = firstRootDirectorySector + numRootDirectorySectors;

	debug_bochs_printf( "First FAT sector = %d\n", firstFATsector );
	debug_bochs_printf( "First ROOT sector = %d\n", firstRootDirectorySector );
	debug_bochs_printf( "First DATA sector = %d\n", firstDataSector );


	uint8_t fat[_bootSector.BytesPerSector * _bootSector.FATSize];
	_pDisk->read(firstFATsector, _bootSector.FATSize, &fat);

	// 0x02600: Root directory
	// 0x04200: Cluster 2 -> boot directory
	// 0x04400: Cluster 3 -> stage1 file
	// 0x04600: Cluster 4 -> stage2 file (198 clusters)
	// 0x1D200: Cluster 202 -> kernel.bin file

#if 0
	for (size_t i = 0; i < sizeof(fat) / 3; ++i)
	{
		int f0 = fat[i * 3];
		int f1 = fat[i * 3 + 1];
		int f2 = fat[i * 3 + 2];

		int v = (f0) | ((f1 << 8) & 0xF00);

		debug_bochs_printf("%03X -> %03X ", i * 2, v);

		v = (f2 << 4) | ((f1 >> 4) & 0x00F);

		debug_bochs_printf("%03X -> %03X ", i * 2 + 1, v);

		if (i % 4 == 0)
			debug_bochs_printf("\n");
	}

	debug_bochs_memdump(fat, sizeof(fat));
#endif
}

FAT12FileSystem::~FAT12FileSystem()
{
}

int FAT12FileSystem::sector_from_cluster(int cluster)
{
	uint32_t root_sector = _bootSector.ReservedSectorCount + _bootSector.NumFATs * _bootSector.FATSize;

	if (cluster == 0)
	{
		// Root directory
		return root_sector;
	}
	else if (cluster == 1)
	{
		panic("There is no cluster 1 in FAT12");
	}
	else
	{
		uint32_t cluster_2 = root_sector + 14;

		return cluster_2 + (cluster - 2) * _bootSector.SectorsPerCluster;
	}	
}

FAT12FileSystem* FAT12FileSystem::tryCreate(drv::itf::IBlockDevice* pDisk, void* pBootSectorData)
{
	auto bootSector = (lowlevel::FAT12_BootSector*)pBootSectorData;
	if( bootSector )
	{
		if( strncmp( bootSector->FileSystemType, "FAT12", 5 ) == 0 )
		{
			return new FAT12FileSystem( pDisk, bootSector );
		}
	}

	return 0;
}

IDirectoryEnum* FAT12FileSystem::root_directory()
{
	//unsigned int firstFATsector = _bootSector.ReservedSectorCount;
	//unsigned int firstRootDirectorySector = firstFATsector + _bootSector.NumFATs * _bootSector.FATSize;
	unsigned int numRootDirectorySectors = (_bootSector.RootEntityCount * sizeof(lowlevel::FAT12_DirectoryEntry)) / _bootSector.BytesPerSector;

	return new FAT12DirectoryEnum(_pDisk, sector_from_cluster(0), numRootDirectorySectors);
}