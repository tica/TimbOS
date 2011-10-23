
#ifndef _DRIVERBASE_H_INC_
#define _DRIVERBASE_H_INC_

namespace drv
{
	namespace itf
	{
		struct IDriverBase
		{
			virtual ~IDriverBase() {}

			virtual const char*	description() = 0;

			virtual void		init() = 0;			
		};
	}
}

#endif // _DRIVERBASE_H_INC_
