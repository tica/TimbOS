
#include "syscall.h"

#include "debug.h"
#include "irq.h"
#include "scheduler.h"

extern "C" void int21_0( int no );

cpu_state* syscall_handler( cpu_state* s )
{
	switch( (syscall::ID)s->eax )
	{
	case syscall::ID::Yield:
		return scheduler::task_yield( s );
	default:
		debug_bochs_printf( "syscall(%d)\n", s->eax );
		return s;
	}		
}

void syscall::init( void )
{
	irq::install_handler( 1, syscall_handler );
}

void syscall::execute( ID no )
{
	int21_0( (int)no );
}
