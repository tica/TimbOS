
#include "system.h"
#include "scheduler.h"

#include "debug.h"
#include "gdt.h"
#include "irq.h"
#include "mm.h"

#include <list>
#include <cstddef>
#include <cstring>

enum TASK_STATE
{
	TASK_STATE_INVALID	= 0x00,
	TASK_STATE_RUNNING	= 0x01,
	TASK_STATE_WAITING	= 0x02
};

struct Task
{
	void*				kernel_stack;
	struct cpu_state*	cpu_state;

	Task( void* kstack, struct cpu_state* cpu )
		:	kernel_stack( kstack ),
			cpu_state( cpu )
	{
	}
};

static std::list<Task*>				s_task_list;
static std::list<Task*>::iterator	s_current_task_it = s_task_list.end();


namespace scheduler
{
	cpu_state*	next(cpu_state* regs);
}

void	scheduler::init()
{		
	irq::install_handler( 0, scheduler::next );
}

cpu_state*	scheduler::next(cpu_state* regs)
{
	if( s_task_list.empty() )
		return regs;

	if( s_current_task_it != s_task_list.end() )
	{
		(*s_current_task_it)->cpu_state = regs;
	}
	
	if( ++s_current_task_it == s_task_list.end() )
		s_current_task_it = s_task_list.begin();

	gdt::update_tss_esp0( (*s_current_task_it)->kernel_stack );

	return (*s_current_task_it)->cpu_state;
}

void scheduler::new_task( void* user_stack, void* entry )
{
	uint8_t* kernel_stack = (uint8_t*)mm::alloc_pages();
	uint8_t* kernel_stack_end = kernel_stack + 4096;
	uint8_t* user_stack_end = ((uint8_t*)user_stack) + 4096;

	struct cpu_state* regs = reinterpret_cast<struct cpu_state*>( kernel_stack_end - sizeof(struct cpu_state) );
	memset( regs, 0, sizeof(regs) );

	regs->eip = (unsigned int)entry;
	regs->esp = (unsigned int)regs;
	regs->useresp = (unsigned int)user_stack_end;

#if 1
	regs->cs = 0x18 | 0x03;
	regs->ss = 0x20 | 0x03;
	regs->ds = 0x20 | 0x03;
	regs->es = 0x20 | 0x03;
	regs->fs = 0x20 | 0x03;
	regs->gs = 0x20 | 0x03;
#else
	regs->cs = 0x08;
	regs->ss = 0x10;
	regs->ds = 0x10;
	regs->es = 0x10;
	regs->fs = 0x10;
	regs->gs = 0x10;
#endif

	regs->eflags = 0x0202;

	Task* task = new Task( kernel_stack_end, regs );
	s_task_list.push_back( task );
}

cpu_state*	scheduler::task_yield( cpu_state* s )
{
	return next( s );
}