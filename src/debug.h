
#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "io.h"
#include "format.h"

#define bochs_console_putc(c) outportb( 0xe9, c )

static inline void output_bochs_char( char ch, void* p )
{
	p = p;
	bochs_console_putc( ch );
}

#define debug_bochs_printf( ... ) format_string( output_bochs_char, 0, __VA_ARGS__ )

#define TRACE() debug_bochs_printf( "%s()\n", __FUNCTION__ )
#define TRACE1( p ) debug_bochs_printf( "%s( %s=%p )\n", __FUNCTION__, #p, p )
#define TRACE2( p, q ) debug_bochs_printf( "%s( %s=%p, %s=%p )\n", __FUNCTION__, #p, p, #q, q )
#define TRACE3( p, q, r ) debug_bochs_printf( "%s( %s=%p, %s=%p, %s=%p )\n", __FUNCTION__, #p, p, #q, q, #r, r )

#endif // _DEBUG_H_
