
#include "memory.h"

#include "debug.h"

void* memcpy( void* destination, const void* source, size_t count )
{
	unsigned char* dest = destination;
	const unsigned char* src = source;
	
	while( count-- != 0 )
	{
		*dest++ = *src++;
	}
	
	return destination;
}

void* memset( void* destination, unsigned char value, size_t count )
{
	//debug_bochs_printf( "memset to %x, set %x (%x times)\n", destination, value, count );

	unsigned char* dest = destination;
	
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
