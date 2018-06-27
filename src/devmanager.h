
#ifndef _DEVMANAGER_H_INC_
#define _DEVMANAGER_H_INC_

#include_next <stdio.h>
#include <cstddef>
#include <list>
#include <string>

#include "driverbase.h"
#include "blockdevice.h"
#include "diskcache.h"

namespace drv
{
	class DeviceManager
	{
		struct DriverTreeNode
		{
			std::string									name;
			itf::IDriverBase*							driver;
			std::list<std::unique_ptr<DriverTreeNode>>	children;

			DriverTreeNode(std::string n, itf::IDriverBase* ptr)
				: name{ std::move(n) }
				, driver{ ptr }
			{
			}
		};

		std::list<std::unique_ptr<DriverTreeNode>>	_devices;

	public:
		void	add_device( itf::IDriverBase* dev, const char* name );
		void	add_device( itf::IBlockDevice* dev, const char* name );

		itf::IBlockDevice* get(const std::string& path);
	};
}

#endif // _DEVMANAGER_H_INC_
