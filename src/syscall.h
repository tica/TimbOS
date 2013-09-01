
#ifndef _SYSCALL_H_INC_
#define _SYSCALL_H_INC_

namespace syscall
{
	void init( void );

	enum class ID
	{
		Yield
	};

	void execute( ID no );	
}

#endif // _SYSCALL_H_INC_
