
#ifndef _INTERLOCKED_H_INC_
#define _INTERLOCKED_H_INC_

#ifdef __cplusplus
extern "C"
{
#endif

	uint32_t	interlocked_exchange( uint32_t* p, uint32_t x );
	uint32_t	interlocked_compare_exchange( uint32_t* p, uint32_t comp, uint32_t exch );
	uint64_t	interlocked_compare_exchange_64( uint64_t* p, uint64_t comp, uint64_t exch );

	uint32_t	interlocked_increment( uint32_t* p );
	uint32_t	interlocked_decrement( uint32_t* p );

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

	template<typename T>
	inline bool compare_exchange( T* p, T comp, T exch )
	{
		static_assert(
			sizeof(T) == 4 ||
			sizeof(T) == 8,
			"Invalid sizeof(T) for compare_exchange");

		switch( sizeof(T) )
		{
		case 4:
			return compare_exchange( reinterpret_cast<uint32_t*>(p), *reinterpret_cast<uint32_t*>(&comp), *reinterpret_cast<uint32_t*>(&exch) );
		case 8:
			return compare_exchange( reinterpret_cast<uint64_t*>(p), *reinterpret_cast<uint64_t*>(&comp), *reinterpret_cast<uint64_t*>(&exch) );
		}
	}

	inline uint32_t increment( uint32_t* p )
	{
		return interlocked_increment( p );
	}

	inline uint32_t decrement( uint32_t* p )
	{
		return interlocked_decrement( p );
	}

	inline uint64_t increment( uint64_t* p )
	{
		uint64_t old;

		do
		{
			old = *p;
		}
		while( !compare_exchange(p, old, old + 1) );

		return old + 1;
	}

	inline uint64_t decrement( uint64_t* p )
	{
		uint64_t old;

		do
		{
			old = *p;
		}
		while( !compare_exchange(p, old, old - 1) );

		return old - 1;
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