
#include "memory.h"
#include "../../debug.h"

void* memcpy( void* destination, const void* source, size_t count )
{
	unsigned char* dest = reinterpret_cast<unsigned char*>( destination );
	const unsigned char* src = reinterpret_cast<const unsigned char*>( source );
	
	while( count-- != 0 )
	{
		*dest++ = *src++;
	}
	
	return destination;
}

void* memmove( void* destination, const void* source, size_t count )
{
	return memcpy( destination, source, count );
}

void* memset( void* destination, unsigned char value, size_t count )
{
	unsigned char* dest = reinterpret_cast<unsigned char*>( destination );
	
	while( count-- )
	{
		*dest++ = value;
	}
	
	return destination;
}

char* strcat( char* destination, char* source )
{
	char* dest = destination;
	
	while( *source )
	{
		*dest++ = *source++;
	}
	
	*dest = '\0';
	
	return destination;
}

int strncmp( const char* s1, const char* s2, size_t count )
{
	TRACE3( s1, s2, count );

	while( count-- > 0 )
	{
		if( *s1 != *s2 )
			return *s1 - *s2;

		if( !*s1 )
			return -1;

		if( !*s2 )
			return 1;
	}

	return 0;
}
