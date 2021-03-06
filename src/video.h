
#ifndef _VIDEO_H_
#define _VIDEO_H_

#include "system.h"

enum CONSOLE_COLOR
{
	CONSOLE_COLOR_BLACK = 0,
	CONSOLE_COLOR_BLUE,
	CONSOLE_COLOR_GREEN,
	CONSOLE_COLOR_AQUA,
	CONSOLE_COLOR_RED,
	CONSOLE_COLOR_PURPLE,
	CONSOLE_COLOR_YELLOW,
	CONSOLE_COLOR_WHITE,
	CONSOLE_COLOR_GRAY,
	CONSOLE_COLOR_LIGHT_BLUE,
	CONSOLE_COLOR_LIGHT_GREEN,
	CONSOLE_COLOR_LIGHT_AQUA,
	CONSOLE_COLOR_LIGHT_RED,
	CONSOLE_COLOR_LIGHT_PURPLE,
	CONSOLE_COLOR_LIGHT_YELLOW,
	CONSOLE_COLOR_BRIGHT_WHITE
};

struct VIDEO_CHAR
{
	char ch;
	unsigned char color;
};

EXTERN_C_BEGIN
	void video_print( struct VIDEO_CHAR* destination, const char* string, unsigned char color );

	void video_clear( struct VIDEO_CHAR* destination );
	void video_display( const struct VIDEO_CHAR* screen );

	void video_scroll( size_t count );

	void video_move_cursor( unsigned int col, unsigned int row );
	void video_config_cursor( bool blink, unsigned int start_row, unsigned int end_row );
EXTERN_C_END

#endif // _VIDEO_H_
