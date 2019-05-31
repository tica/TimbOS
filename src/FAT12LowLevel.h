#pragma once

#include "system.h"

#include <cstdint>

namespace fs
{
	namespace fat12
	{
		namespace lowlevel
		{
			struct __ATTRIBUTE_PACKED__ FAT12_BootSector
			{
				uint16_t	JmpBootLow;
				uint8_t		JmpBootHigh;
				char		OSName[8];
				uint16_t	BytesPerSector;
				uint8_t		SectorsPerCluster;
				uint16_t	ReservedSectorCount;
				uint8_t		NumFATs;
				uint16_t	RootEntityCount;
				uint16_t	TotalSectorCount;
				uint8_t		MediaType;
				uint16_t	FATSize;
				uint16_t	SectorsPerTrack;
				uint16_t	NumHeads;
				uint32_t	HiddenSectors;
				uint32_t	TotalSectors32;
				uint8_t		DriveNumber;
				uint8_t		Reserved0;
				uint8_t		Reserved1;
				uint32_t	VolumeID;
				uint8_t		Reserved2[11];
				char		FileSystemType[8];
				uint8_t		Reserved3[450];
			};

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
}