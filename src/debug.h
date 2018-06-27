
#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "io.h"
#include "format.h"


void output_bochs_char( char ch, void* p );
void debug_bochs_memdump( const void* addr, unsigned int length );
[[noreturn]] void panic( const char* txt, ... );

#define debug_bochs_printf( ... ) format_string( output_bochs_char, 0, __VA_ARGS__ )

#define TRACE() debug_bochs_printf( "%s()\n", __FUNCTION__ )
#define TRACE1( p ) debug_bochs_printf( "%s( %s=%p )\n", __FUNCTION__, #p, p )
#define TRACE2( p, q ) debug_bochs_printf( "%s( %s=%p, %s=%p )\n", __FUNCTION__, #p, p, #q, q )
#define TRACE3( p, q, r ) debug_bochs_printf( "%s( %s=%p, %s=%p, %s=%p )\n", __FUNCTION__, #p, p, #q, q, #r, r )
#define DUMP( addr, len ) debug_bochs_memdump( addr, len )
#define DUMP_OBJ( obj ) debug_bochs_memdump( &(obj), sizeof(obj) )
#define PANIC( txt ) do { debug_bochs_printf( "PANIC (%s): %s\n", __FUNCTION__, txt ); for(;;); } while(0)

#endif // _DEBUG_H_
