
#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "system.h"

struct CONSOLE
{
private:
	size_t		buffer_width;
	size_t		buffer_height;
	
	struct VIDEO_CHAR*	buffer;
	
	size_t		cursor_column;
	size_t		cursor_row;

	int			visible;
	size_t		top_row;

	static void write_char_to_console( char ch, void* ptr );

public:
	CONSOLE();
public:
	void printf( const char* format, ... );
	void printn( const char ch, size_t count );
	void dump( const void* mem, size_t len );

private:
	void move_cursor( int x, int y );
};

extern struct CONSOLE console;

#endif // _CONSOLE_H_
