
#ifndef _DEVMANAGER_H_INC_
#define _DEVMANAGER_H_INC_

#include <list>

#include "driverbase.h"

namespace drv
{
	class DeviceManager
	{
		struct DriverTreeNode
		{
			itf::IDriverBase*			driver;
			std::list<DriverTreeNode*>	children;
		};

		std::list<DriverTreeNode*>	_devices;

	public:
		void	add_device( itf::IDriverBase* dev )
		{
			(void)dev;
		}
	};
}

#endif // _DEVMANAGER_H_INC_
