// testing.cpp : Defines the entry point for the console application.
//

#include <Windows.h>

#if 0
#include "../src/lib/mystl/string"
#define _XSTRING_
#else
#include <string>
#endif

#include <stdio.h>
#include <map>

int main(int argc, char* argv[])
{
	std::string s( "Aaaa" );
	printf( "s = %s\n", s.c_str() );
	s = "Bbb";
	printf( "s = %s\n", s.c_str() );

	std::string t = std::move(s);
	printf( "s = %s\n", s.c_str() );
	printf( "t = %s\n", t.c_str() );

	std::map<std::string, int> m;

	m["Aaa"] = 1;
	m["bbbb"] = 2;
	m["cc"] = 3;

	for( auto& i : m )
	{
		printf( "%s -> %d\n", i.first.c_str(), i.second );
	}

	/*
	unsigned int x = 3;
	unsigned int y = ::InterlockedIncrement( &x );

	unsigned int z = 0;
	unsigned int z0 = ::InterlockedExchange( &z, 1 );
	*/

	//unsigned int x = 2;
	//unsigned int y = ::InterlockedCompareExchange( &x, 3, 2 );

	unsigned int x = 3;
	unsigned int y = ::InterlockedIncrement( &x );

	return 0;
}

