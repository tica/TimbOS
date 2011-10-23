
#ifndef _DRV_H_INC_
#define _DRV_H_INC_

namespace drv
{
	class DeviceManager;
	typedef void (*devprobe_func)( drv::DeviceManager& devmgr );

	namespace manager
	{
		void	reg_probe( drv::devprobe_func probe_func );
		void	probe_all( DeviceManager& devmgr );

		struct REGISTER_DRIVER
		{
			REGISTER_DRIVER( devprobe_func func )
			{
				reg_probe( func );
			}
		};
	};

#define CONCAT_IMPL( x, y ) x##y
#define MACRO_CONCAT( x, y ) CONCAT_IMPL( x, y )
#define REGISTER_DRIVER_PROBE(func) manager::REGISTER_DRIVER MACRO_CONCAT(_probe_reg,__COUNTER__)(func)
}

#endif
