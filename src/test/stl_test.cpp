
#include "../debug.h"

#include <utility>
#include <list>
#include <functional>
#include <vector>
#include <tuple>
#include <iterator>
#include <algorithm>
#include <map>
#include <set>
#include <string>

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

	void test_vector( void )
	{
		std::vector<int> v;

		for( int i = 0; i < 1000; ++i )
			v.push_back( i );

		v.clear();
	}

	void test_tuple( void )
	{
		int x, y;
		std::tie(x, y) = std::make_tuple( 1, 0 );
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

	void test_pair( void )
	{
		auto p = std::make_pair( 1, 2 );

		debug_bochs_printf( "pair: %d-%d\n", p.first, p.second );
	}

	void test_map( void )
	{
		std::map<int, int> m;
		for( int i = 0; i < 10; ++i )
			m[i] = 10 - i;

		for( auto p : m )
		{
			debug_bochs_printf( "%d -> %d\n", p.first, p.second );
		}
	}

	void test_set( void )
	{
		std::set<int> s;
		for( int i = 0; i < 10; ++i ) s.insert( i );

		debug_bochs_printf( "set: { " );
		for( auto i : s )
		{
			debug_bochs_printf( "%d ", i );
		}

		debug_bochs_printf( "}\n" );
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
}

void test_stl( void )
{
	/*
	repeat( test_iterator, 10 );
	repeat( test_tuple, 10 );
	repeat( test_pair, 10 );	
	repeat( test_set, 1 );
	repeat( test_list, 10 );
	repeat( test_map, 1 );
	*/

	test_string();

	//repeat( test_vector, 10 );	
}