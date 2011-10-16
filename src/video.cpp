
#include "video.h"
#include "lib/memory.h"
#include "debug.h"
#include "io.h"

struct VIDEO_CHAR* video_ram = (struct VIDEO_CHAR*)(0xB8000+ 0xC0000000);

#define	SCREEN_WIDTH	80
#define	SCREEN_HEIGHT	25
#define	SCREEN_SIZE		(SCREEN_WIDTH * SCREEN_HEIGHT)

int video_cursor_x = 0;
int video_cursor_y = 0;

void video_print( struct VIDEO_CHAR* destination, const char* string, unsigned char color )
{
	while( *string )
	{
		destination->ch = *string++;
		destination->color = color;		
		destination++;
	}
}

void video_clear( struct VIDEO_CHAR* destination )
{
	size_t count = SCREEN_SIZE;
	while( count-- )
	{
		destination->ch = destination->color = 0;
		destination++;
	}
}

void video_display( const struct VIDEO_CHAR* screen )
{
	memcpy( video_ram, screen, SCREEN_SIZE * sizeof(struct VIDEO_CHAR) );
}

void video_scroll( size_t count )
{
	size_t screen_size = SCREEN_SIZE * sizeof(struct VIDEO_CHAR);
	size_t black_size = count * SCREEN_WIDTH * sizeof(struct VIDEO_CHAR);
	
	memcpy( video_ram, video_ram + SCREEN_WIDTH * count, screen_size - black_size );
	memset( ((uint8_t*)video_ram) + (screen_size - black_size), 0x20, black_size );
}

void video_move_cursor( unsigned int col, unsigned int row )
{
	unsigned int p = row * 80 + col;
	
	outportb( 0x3D4, 15 );
	outportb( 0x3D5, p & 0xff );
	outportb( 0x3D4, 14 );
	outportb( 0x3D5, (p >> 8) & 0xff );	
}

void video_config_cursor( bool blink, unsigned int start_row, unsigned int end_row )
{
	outportb( 0x3D4, 10 );
	outportb( 0x3D5, (blink ? 0x40 : 0x00) | (start_row & 0x1f) );
	outportb( 0x3D4, 11 );
	outportb( 0x3D5, 0x00 | (end_row & 0x1f) );
}