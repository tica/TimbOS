
#ifndef _IRQ_H_INC_
#define _IRQ_H_INC_

#include "system.h"

namespace irq
{
	typedef struct cpu_state* (*handler_proc)(struct cpu_state*);

	void init( void );	
	void install_handler( int irq, handler_proc handler );
	void uninstall_handler( int irq );
}

#endif
