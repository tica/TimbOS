
#ifndef _IRQ_H_INC_
#define _IRQ_H_INC_

#include "system.h"

namespace irq
{
	typedef void (*handler_proc)(struct regs*);

	void init( void );
	void install_handler( int irq, handler_proc handler );
}

#endif
