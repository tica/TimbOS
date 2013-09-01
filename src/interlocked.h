
#ifndef _INTERLOCKED_H_INC_
#define _INTERLOCKED_H_INC_

#ifdef __cplusplus
extern "C"
{
#endif

	uint32_t interlocked_exchange( uint32_t* p, uint32_t x );
	uint32_t interlocked_compare_exchange( uint32_t* p, uint32_t comp, uint32_t exch );
	uint64_t interlocked_compare_exchange_64( uint64_t* p, uint64_t comp, uint64_t exch );

#ifdef __cplusplus
}
#endif

namespace interlocked
{
	inline uint32_t exchange( uint32_t* p, uint32_t x )
	{
		return interlocked_exchange( p, x );
	}

	inline bool compare_exchange( uint32_t* p, uint32_t comp, uint32_t exch )
	{
		return interlocked_compare_exchange( p, comp, exch ) == comp;
	}

	inline bool compare_exchange( uint64_t* p, uint64_t comp, uint64_t exch )
	{
		return interlocked_compare_exchange_64( p, comp, exch ) == comp;
	}

	inline void enter( uint32_t* cs )
	{
		while( exchange( cs, 1 ) > 0 )
			;
	}

	inline void leave( uint32_t* cs )
	{
		exchange( cs, 0 );
	}
}

#endif // _INTERLOCKED_H_INC_