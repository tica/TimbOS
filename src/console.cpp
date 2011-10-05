
#include "console.h"
#include "system.h"
#include "video.h"
#include "format.h"
#include "memory.h"

CONSOLE::CONSOLE()
{
	static struct VIDEO_CHAR s_buffer[80 * 25 * 100] = {};
	for( size_t i = 0; i < _countof(s_buffer); ++i )
	{
		s_buffer[i].ch = ' ';
		s_buffer[i].color = 0x07;
	}
	
	buffer_width = 80;
	buffer_height = 100 * 25;
	buffer = s_buffer;
	cursor_row = 0;
	cursor_column = 0;
	visible = 1;
	top_row = 0;

	video_config_cursor( true, 0x0d, 0x0f );
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

	if( console->cursor_row < console->top_row )
	{
		// Cursor before visible area: move visible start to cursor

		console->top_row = console->cursor_row;
	}
	else
	{		
		if( (console->cursor_row - console->top_row) >= 25 )
		{
			// Cursor after visible area: move visible start down
			console->top_row = console->cursor_row - 24;
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
		video_move_cursor( this->cursor_column, this->cursor_row - this->top_row );
	}
}


 
