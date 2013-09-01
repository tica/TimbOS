
#ifndef _SCHEDULER_H_INC_
#define _SCHEDULER_H_INC_

namespace scheduler
{
	void	init();

	void	new_task( void* user_stack, void* entry );

	cpu_state*	task_yield( cpu_state* s );
}

#endif
