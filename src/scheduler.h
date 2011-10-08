
#ifndef _SCHEDULER_H_INC_
#define _SCHEDULER_H_INC_

namespace scheduler
{
	struct cpu_state* next(struct cpu_state* regs);
	void new_task( void* stack, void* user_stack, void* entry );
}

#endif
