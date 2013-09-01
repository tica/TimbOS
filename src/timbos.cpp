
#include "timbos.h"

#include "syscall.h"

void scheduler::yield()
{
	syscall::execute( syscall::ID::Yield );
}
