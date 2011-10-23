
#include "system.h"
#include "drv.h"

static drv::devprobe_func s_probe_funcs[16];
static unsigned int s_probe_func_count = 0;

void	drv::manager::reg_probe( drv::devprobe_func probe_func )
{
	s_probe_funcs[s_probe_func_count++] = probe_func;
}

void	drv::manager::probe_all( drv::DeviceManager& devmgr )
{
	for( unsigned int i = 0; i < s_probe_func_count; ++i )
	{
		s_probe_funcs[i]( devmgr );
	}
}
