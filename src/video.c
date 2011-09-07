
#include "video.h"
#include "memory.h"

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
	memset( ((void*)video_ram) + (screen_size - black_size), 0x20, black_size );
}
