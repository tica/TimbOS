
#ifndef _DRV_H_INC_
#define _DRV_H_INC_

namespace drv
{
	class DriverManager;
	typedef void (*devprobe_func)( drv::DriverManager& drvmgr );

	class DriverManager
	{
		static void	regProbeFunc( drv::devprobe_func probe_func );
	public:
		struct REGISTER_DRIVER
		{
			REGISTER_DRIVER( devprobe_func func )
			{
				DriverManager::regProbeFunc( func );
			}
		};

	public:
		void	probe_all();	
	};

#define CONCAT_IMPL( x, y ) x##y
#define MACRO_CONCAT( x, y ) CONCAT_IMPL( x, y )
#define REGISTER_DRIVER_PROBE(func) DriverManager::REGISTER_DRIVER MACRO_CONCAT(_probe_reg,__COUNTER__)(func)
}

#endif
