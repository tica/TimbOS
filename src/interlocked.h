
#ifndef _INTERLOCKED_H_INC_
#define _INTERLOCKED_H_INC_

#ifdef __cplusplus
extern "C"
{
#endif

	int interlocked_exchange( int* p, int x );
	int interlocked_compare_exchange( int* p, int comp, int exch );

#ifdef __cplusplus
}
#endif

namespace interlocked
{
	inline int exchange( int* p, int x )
	{
		return interlocked_exchange( p, x );
	}

	inline bool compare_exchange( int* p, int comp, int exch )
	{
		return interlocked_compare_exchange( p, comp, exch ) == comp;
	}

	inline void enter( int* cs )
	{
		while( exchange( cs, 1 ) > 0 )
			;
	}

	inline void leave( int* cs )
	{
		exchange( cs, 0 );
	}
}

#endif // _INTERLOCKED_H_INC_