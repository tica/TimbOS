
#ifndef _FAT12FILESYSTEM_H_INC_
#define _FAT12FILESYSTEM_H_INC_

#include "fs.h"
#include "blockdevice.h"

namespace fs
{
	namespace lowlevel
	{
		struct __ATTRIBUTE_PACKED__ FAT12_BootSector
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
	}

	class FAT12FileSystem
		:	public IFileSystem
	{
	private:
		drv::itf::IBlockDevice*		_pDisk;

		lowlevel::FAT12_BootSector	_bootSector;

		int sector_from_cluster(int cluster);

	private:
		FAT12FileSystem( drv::itf::IBlockDevice* pDisk, lowlevel::FAT12_BootSector* pBootSector );

	public:
		virtual ~FAT12FileSystem() override;

		static FAT12FileSystem* tryCreate( drv::itf::IBlockDevice* pDisk, void* pBootSectorData );

	public:
		virtual IDirectoryEnum* root_directory() override;
	};
}

#endif // _FAT12FILESYSTEM_H_INC_
