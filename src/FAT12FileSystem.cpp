
#include "FAT12FileSystem.h"
#include "FAT12DirectoryEnumBase.h"

#include "debug.h"
#include "system.h"

#include <string.h>
#include <stdint.h>

#include <string>
#include <string_view>

using namespace fs;


class FAT12RootDirectoryEnum : public fs::fat12::FAT12DirectoryEnumBase
{
	std::vector<uint16_t>	_sectors;

public:
	FAT12RootDirectoryEnum( std::shared_ptr<fs::fat12::FAT12FileSystem> parent, unsigned int first_sector, unsigned int sector_count )
		: fs::fat12::FAT12DirectoryEnumBase( parent  )
	{
		for( unsigned int i = 0; i < sector_count; ++i )
		{
			_sectors.push_back( first_sector + i );
		}
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


fs::fat12::FAT12FileSystem::FAT12FileSystem( drv::itf::IBlockDevice* pDisk, lowlevel::FAT12_BootSector* pBootSector )
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
	unsigned int numRootDirectorySectors = (_bootSector.RootEntityCount * sizeof( lowlevel::FAT12_DirectoryEntry )) / _bootSector.BytesPerSector;
	_firstDataSector = firstRootDirectorySector + numRootDirectorySectors;

	debug_bochs_printf( "First FAT sector = %d\n", firstFATsector );
	debug_bochs_printf( "First ROOT sector = %d\n", firstRootDirectorySector );
	debug_bochs_printf( "First DATA sector = %d\n", _firstDataSector );

	std::vector<uint8_t> fat( _bootSector.BytesPerSector* _bootSector.FATSize );

	_pDisk->read( firstFATsector, _bootSector.FATSize, fat.data() );	

	// 0x02600: Root directory
	// 0x04200: Cluster 2 -> boot directory
	// 0x04400: Cluster 3 -> stage1 file
	// 0x04600: Cluster 4 -> stage2 file (198 clusters)
	// 0x1D200: Cluster 202 -> kernel.bin file

	_fat.read( fat.data(), fat.size() );	

	std::vector<uint8_t> test( fat.size() );

	_fat.write( test.data(), test.size() );

	debug_bochs_printf( "WRITE SUCCESS = %d\n", fat == test );

	//debug_bochs_printf( "\n" );
	//debug_bochs_memdump( fat, sizeof( fat ) );
}

fs::fat12::FAT12FileSystem::~FAT12FileSystem()
{
}

unsigned int fs::fat12::FAT12FileSystem::sector_from_cluster( cluster_id cluster )
{
	uint32_t root_sector = _bootSector.ReservedSectorCount + _bootSector.NumFATs * _bootSector.FATSize;

	if( cluster == 0 )
	{
		// Root directory
		return root_sector;
	}
	else if( cluster == 1 )
	{
		panic( "There is no cluster 1 in FAT12" );
	}
	else
	{
		uint32_t cluster_2 = _firstDataSector;

		return cluster_2 + (cluster - 2) * _bootSector.SectorsPerCluster;
	}
}

std::shared_ptr<fs::fat12::FAT12FileSystem> fs::fat12::FAT12FileSystem::tryCreate( drv::itf::IBlockDevice* pDisk, void* pBootSectorData )
{
	auto bootSector = (lowlevel::FAT12_BootSector*)pBootSectorData;
	if( bootSector )
	{
		if( strncmp( bootSector->FileSystemType, "FAT12", 5 ) == 0 )
		{
			auto fs = std::make_shared<FAT12FileSystem>( pDisk, bootSector );
			fs->_weak_this = fs;
			return fs;
		}
	}

	return nullptr;
}

std::shared_ptr<IDirectoryEnum> fs::fat12::FAT12FileSystem::root_directory()
{
	//unsigned int firstFATsector = _bootSector.ReservedSectorCount;
	//unsigned int firstRootDirectorySector = firstFATsector + _bootSector.NumFATs * _bootSector.FATSize;
	unsigned int numRootDirectorySectors = (_bootSector.RootEntityCount * sizeof( lowlevel::FAT12_DirectoryEntry )) / _bootSector.BytesPerSector;

	auto this_ptr = std::shared_ptr<FAT12FileSystem>( _weak_this );

	return std::make_shared<FAT12RootDirectoryEnum>( this_ptr, sector_from_cluster( 0 ), numRootDirectorySectors );
}