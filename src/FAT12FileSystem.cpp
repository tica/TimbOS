
#include "system.h"
#include "FAT12FileSystem.h"
#include "debug.h"

#include <memory.h>

using namespace fs;

struct __ATTRIBUTE_PACKED__ fs::FAT12_BootSector
{
	unsigned short	JmpBootLow;
	unsigned char	JmpBootHigh;
	char			OSName[8];
	unsigned short	BytesPerSector;
	unsigned char	SectorsPerCluster;
	unsigned short	ReservedSectorCount;
	unsigned char	NumFATs;
	unsigned short	RootEntityCount;
	unsigned short	TotalSectorCount;
	unsigned char	MediaType;
	unsigned short	FATSize;
	unsigned short	SectorsPerTrack;
	unsigned short	NumHeads;
	unsigned int	HiddenSectors;
	unsigned int	TotalSectors32;
	unsigned char	DriveNumber;
	unsigned char	Reserved0;
	unsigned char	Reserved1;
	unsigned int	VolumeID;
	unsigned char	Reserved2[11];
	char			FileSystemType[8];
	unsigned char	Reserved3[450];
};

struct __ATTRIBUTE_PACKED__ FAT12_DirectoryEntry
{
	char			Name[11];
	unsigned char	Attribute;
	unsigned char	Reserved0;
	unsigned char	CreateTimeTenth;
	unsigned short	Reserved1;
	unsigned short	LastAccessDate;
	unsigned short	Reserved2;
	unsigned short	Reserved3;
	unsigned short	WriteTime;
	unsigned short	WriteDate;
	unsigned short	FileSystemClusterLogical;
	unsigned int	FileSize;
};

class FAT12Directory
	:	public IFile
{
	unsigned short _firstCluster;

public:
	FAT12Directory( unsigned short firstCluster )
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

class FAT12DirectoryEnumBase
	:	public IDirectoryEnum
{
	unsigned int	_entry_count;
	unsigned int	_index;

public:
	FAT12DirectoryEnumBase( unsigned int sector_count )
		:	_entry_count( sector_count * 16 ),
			_index( 0 )
	{
	}

	void reset()
	{
		_index = 0;
	}

	bool next( IFile** file )
	{
		(void)file;

		while( _index < _entry_count )
		{
			unsigned int sector_index = _index / 16;
			unsigned int entry_index = _index % 16;

			FAT12_DirectoryEntry* entries = (FAT12_DirectoryEntry*)lock_sector( sector_index );
			FAT12_DirectoryEntry& entry = entries[entry_index];

			switch( entry.Name[0] )
			{
			case '\x000': // empty from here
				_index = _entry_count;
				unlock_sector( sector_index );
				return false;
			case '\x00E': // empty
			case '\x0E5': // deleted
				_index += 1;
				unlock_sector( sector_index );
				continue;
			}

			char buf[13] = {};
			extract_file_name( entry.Name, buf );

			debug_bochs_printf( "entry.Name[0] = %2x\n", entry.Name[0] );
			debug_bochs_printf( "entry.Name = %s\n", buf );
			debug_bochs_printf( "entry.Attribute = %2x\n", entry.Attribute );
			debug_bochs_printf( "entry.FileSize = %d\n", entry.FileSize );
			debug_bochs_printf( "entry.FileSystemClusterLogical = %d\n", entry.FileSystemClusterLogical );

			unlock_sector( sector_index );
			_index += 1;

			return true;
		}

		return false;

		

		return true;
	}

protected:
	virtual void* lock_sector( unsigned int n ) = 0;
	virtual void unlock_sector( unsigned int n ) = 0;
};

class FAT12RootDirectoryEnum
	:	public FAT12DirectoryEnumBase
{
	drv::itf::IDiskCacheBuffer* _pRootDirectoryBuffer;

public:
	FAT12RootDirectoryEnum( drv::itf::IDiskCacheBuffer* pRootDirectoryBuffer )
		:	FAT12DirectoryEnumBase( pRootDirectoryBuffer->block_count() ),
			_pRootDirectoryBuffer( pRootDirectoryBuffer )
	{
	}
private:
	virtual void* lock_sector( unsigned int n )
	{
		return (void*)(uintptr_t(_pRootDirectoryBuffer->lock()) + n * 512);
	}

	virtual void unlock_sector( unsigned int )
	{
		_pRootDirectoryBuffer->unlock();
	}
};

FAT12FileSystem::FAT12FileSystem( drv::itf::IDiskCache* pDisk, drv::itf::IDiskCacheBuffer* pBootSectorBuffer, FAT12_BootSector* pBootSector )
	:	_pDisk( pDisk ),
		_pBootSectorBuffer( pBootSectorBuffer ),
		_pBootSector( pBootSector )
{
	debug_bochs_printf( "Creating FAT12 File system\n" );
	debug_bochs_printf( "OSName = %s\n", _pBootSector->OSName );
	debug_bochs_printf( "BytesPerSector = %d\n", _pBootSector->BytesPerSector );
	debug_bochs_printf( "SectorsPerCluster = %d\n", _pBootSector->SectorsPerCluster );
	debug_bochs_printf( "ReservedSectorCount = %d\n", _pBootSector->ReservedSectorCount );
	debug_bochs_printf( "NumFATs = %d\n", _pBootSector->NumFATs );
	debug_bochs_printf( "RootEntityCount = %d\n", _pBootSector->RootEntityCount );
	debug_bochs_printf( "TotalSectorCount = %d\n", _pBootSector->TotalSectorCount );
	debug_bochs_printf( "MediaType = %2x\n", _pBootSector->MediaType );
	debug_bochs_printf( "FATSize = %d\n", _pBootSector->FATSize );
	debug_bochs_printf( "SectorsPerTrack = %d\n", _pBootSector->SectorsPerTrack );
	debug_bochs_printf( "NumHeads = %d\n", _pBootSector->NumHeads );
	debug_bochs_printf( "HiddenSectors = %d\n", _pBootSector->HiddenSectors );
	debug_bochs_printf( "TotalSectors32 = %d\n", _pBootSector->TotalSectors32 );
	debug_bochs_printf( "DriveNumber = %d\n", _pBootSector->DriveNumber );
	debug_bochs_printf( "VolumeID = %x\n", _pBootSector->VolumeID );

	unsigned int firstFATsector = _pBootSector->ReservedSectorCount;
	unsigned int firstRootDirectorySector = firstFATsector + _pBootSector->NumFATs * _pBootSector->FATSize;
	unsigned int numRootDirectorySectors = (_pBootSector->RootEntityCount * sizeof(FAT12_DirectoryEntry)) / _pBootSector->BytesPerSector;
	unsigned int firstDataSector = firstRootDirectorySector + numRootDirectorySectors;

	debug_bochs_printf( "First FAT sector = %d\n", firstFATsector );
	debug_bochs_printf( "First ROOT sector = %d\n", firstRootDirectorySector );
	debug_bochs_printf( "First DATA sector = %d\n", firstDataSector );

	_pFATBuffer = _pDisk->prepare( firstFATsector, _pBootSector->FATSize );
	_pRootBuffer = _pDisk->prepare( firstRootDirectorySector, numRootDirectorySectors );
}

FAT12FileSystem::~FAT12FileSystem()
{
	_pBootSectorBuffer->unlock();
}

FAT12FileSystem* FAT12FileSystem::tryCreate( drv::itf::IDiskCache* pDisk, drv::itf::IDiskCacheBuffer* pBootSectorBuffer )
{
	FAT12_BootSector* bootSector = (FAT12_BootSector*)pBootSectorBuffer->lock();
	if( bootSector )
	{
		if( strncmp( bootSector->FileSystemType, "FAT12", 5 ) == 0 )
		{
			return new FAT12FileSystem( pDisk, pBootSectorBuffer, bootSector );
		}

		pBootSectorBuffer->unlock();
	}

	return 0;
}

IDirectoryEnum* FAT12FileSystem::root_directory()
{
	return new FAT12RootDirectoryEnum( _pRootBuffer );
}