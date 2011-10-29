
#include "system.h"
#include "devmanager.h"

#include "diskcacheimpl.h"

#include "console.h"

static drv::itf::IDiskCache* s_floppy0_cache = 0;


void	drv::DeviceManager::add_device( itf::IDriverBase* dev )
{
	console.printf( "DeviceManager: Added device (%s)\n", dev->description() );
}

void	drv::DeviceManager::add_device( itf::IBlockDevice* dev )
{
	console.printf( "DeviceManager: Added device (%s)\n", dev->description() );

	s_floppy0_cache = new drv::DiskCache( dev );

	console.printf( "DeviceManager: Added device (%s)\n", s_floppy0_cache->description() );
}

drv::itf::IDiskCache*	drv::DeviceManager::floppy0_cache()
{
	return s_floppy0_cache;
}
