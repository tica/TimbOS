
#include "../debug.h"

#include <functional>
#include <utility>
#include <list>
#include <set>
#include <vector>
#include <map>
#include <tuple>
#include <iterator>
#include <string>

#if 0
include <algorithm>
#include <memory>
#endif

namespace
{
	void repeat( std::function<void()> f, unsigned int count )
	{
		while( count-- > 0 )
		{
			f();
		}
	}

	void test_list( void )
	{
		std::list<int> lst;

		for( int i = 0; i < 10; ++i )
			lst.push_back( i );

		lst.clear();
	}

	void test_set(void)
	{
		std::set<int> s;
		for (int i = 0; i < 10; ++i) s.insert(i);

		debug_bochs_printf("set: { ");
		for (auto i : s)
		{
			debug_bochs_printf("%d ", i);
		}

		debug_bochs_printf("}\n");
	}

	void test_vector(void)
	{
		std::vector<int> v;

		for (int i = 0; i < 1000; ++i)
			v.push_back(i);

		v.clear();
	}

	void test_map(void)
	{
		std::map<int, int> m;
		for (int i = 0; i < 10; ++i)
			m[i] = 10 - i;

		for (auto p : m)
		{
			debug_bochs_printf("%d -> %d\n", p.first, p.second);
		}
	}

	void test_tuple(void)
	{
		int x, y;
		std::tie(x, y) = std::make_tuple(1, 0);

		debug_bochs_printf("tuple: (%d %d)\n", x, y);
	}

	void test_iterator( void )
	{
		debug_bochs_printf( "test_iterator:\n" );

		std::vector<int> v;
		//for( int i = 0; i < 10; ++i ) v.push_back(i);
		int n = 0;
		std::generate_n( std::back_inserter(v), 10, [&n] () -> int { return ++n; } );

		for( auto i = std::begin(v); i != std::end(v); ++i )
			debug_bochs_printf( "%d ", *i );

		for( auto i : v )
			debug_bochs_printf( "%d ", i );

		debug_bochs_printf( "\n" );
	}

	void test_pair(void)
	{
		auto p = std::make_pair(1, 2);

		debug_bochs_printf("pair: %d-%d\n", p.first, p.second);
	}

	void test_string( void )
	{
		debug_bochs_printf( "\n<<BEGIN test_string\n" );

		std::map<std::string, int> m;

		m["ddd"] = 4;
		m["Aaa"] = 1;
		m["bbbb"] = 2;
		m["cc"] = 3;		

		for( auto& i : m )
		{
			debug_bochs_printf( "%s -> %d\n", i.first.c_str(), i.second );
		}

		debug_bochs_printf( "\n>>END test_string\n\n" );
	}

#if 1
	void test_shared_ptr( void )
	{
		debug_bochs_printf( "\n<<BEGIN test_shared_ptr\n" );

		std::shared_ptr<int> p( new int(1) );
		std::shared_ptr<int> q( new int(2) );

		debug_bochs_printf( "*p = %d, *q = %d\n", *p, *q );
		q = p;
		debug_bochs_printf( "*p = %d, *q = %d\n", *p, *q );

		p.reset();

		std::weak_ptr<int> w(q);

		std::shared_ptr<int> s = w.lock();

		debug_bochs_printf("s present = %d\n", s != nullptr);
		debug_bochs_printf("*q = %d, *s = %d\n", *q, *s);

		s.reset();
		q.reset();

		s = w.lock();

		debug_bochs_printf("s present = %d\n", s != nullptr);

		debug_bochs_printf( "\n>>END test_shared_ptr\n\n" );
	}
#endif
}

void test_stl( void )
{
	repeat(test_list, 1);
	repeat(test_set, 1);
	repeat(test_vector, 1);
	repeat(test_map, 1);
	repeat(test_tuple, 10);
	repeat(test_iterator, 10);
	repeat(test_pair, 10);

	test_string();

	test_shared_ptr();
}
