
#include "FAT12DirectoryEnum.h"

#include "FAT12Directory.h"

std::shared_ptr<fs::IDirectoryEnum> fs::fat12::FAT12Directory::children()
{
	return std::make_shared<FAT12DirectoryEnum>( _fs, _firstCluster );
}