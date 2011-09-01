
#include "console.h"
#include "system.h"
#include "video.h"
#include "format.h"

CONSOLE::CONSOLE()
{
	static struct VIDEO_CHAR s_buffer[100*80*25];
	
	buffer_width = 80;
	buffer_height = 100;
	buffer = s_buffer;
	cursor_row = 0;
	cursor_column = 0;
	visible = 1;
	top_row = 0;
}


void CONSOLE::write_char_to_console( char ch, void* ptr )
{
	struct CONSOLE* console = (struct CONSOLE*)ptr;

	if( ch == '\n' )
	{
		console->cursor_column = 0;
		console->cursor_row += 1;
	}
	else
	{
		struct VIDEO_CHAR* dest = console->buffer + console->buffer_width * console->cursor_row + console->cursor_column;
		dest->ch = ch;
		dest->color = 0x07;

		console->cursor_column += 1;
		if( console->cursor_column >= console->buffer_width )
		{
			console->cursor_row += 1;
			console->cursor_column = 0;
		}
	}
}

void CONSOLE::printf( const char* format, ... )
{
	va_list ap;
	va_start( ap, format );	
	format_string_varg( CONSOLE::write_char_to_console, (void*)this, format, ap );	
	va_end( ap );

	if( this->visible )
	{		
		video_display( this->buffer + this->top_row * this->buffer_width );
	}
}
