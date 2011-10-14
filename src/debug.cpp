
#include "debug.h"

#include "io.h"

#define bochs_console_putc(c) outportb( 0xe9, c )

void output_bochs_char( char ch, void* p )
{
	p = p;
	bochs_console_putc( ch );
}

bool isprintable( char ch )
{
	return ch >= 0x20 && ch <= 0x7E;
}

void debug_bochs_memdump( void* address, unsigned int length )
{
	unsigned char* begin = (unsigned char*)address;
	unsigned char* end = begin + length - 1;

	uintptr_t first_addr = ((uintptr_t)begin) & ~0xF;
	uintptr_t last_addr = ((uintptr_t)end) & ~0xF;	

	for( uintptr_t line_addr = first_addr; line_addr <= last_addr; line_addr += 16 )
	{
		debug_bochs_printf( "%p ", line_addr );

		unsigned char* ptr = (unsigned char*)line_addr;
		for( unsigned int i = 0; i < 16; ++i )
		{
			if( ptr >= begin && ptr <= end )
				debug_bochs_printf( "%2x ", *ptr );
			else
				debug_bochs_printf( "?? ", *ptr );

			ptr++;
		}

		debug_bochs_printf( "  " );

		ptr = (unsigned char*)line_addr;
		for( unsigned int i = 0; i < 16; ++i )
		{
			if( ptr >= begin && ptr <= end )
				if( isprintable( *ptr ) )
					debug_bochs_printf( "%c", *ptr );
				else
					debug_bochs_printf( ".", *ptr );
			else
				debug_bochs_printf( " ", *ptr );

			ptr++;
		}

		debug_bochs_printf( "\n" );
	}
}
