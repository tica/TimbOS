
#include "system.h"
#include "devmanager.h"

#include "console.h"



void	drv::DeviceManager::add_device( itf::IDriverBase* dev, const char* name )
{
	auto node = std::make_unique<DriverTreeNode>( name, dev );

	_devices.push_back(std::move(node));

	console.printf( "DeviceManager: Added device %s - (%s)\n", name, dev->description() );
}

void	drv::DeviceManager::add_device( itf::IBlockDevice* dev, const char* name)
{
	auto node = std::make_unique<DriverTreeNode>(name, dev);

	_devices.push_back( std::move(node) );

	console.printf( "DeviceManager: Added device %s - (%s)\n", name, dev->description() );
}

drv::itf::IBlockDevice* drv::DeviceManager::get(const std::string& path)
{
	for (auto&& node : _devices)
	{
		if (node->name == path)
		{
			return reinterpret_cast<drv::itf::IBlockDevice*>(node->driver);
		}
	}

	return nullptr;
}
