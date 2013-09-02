
#include <stddef.h>

void* memcpy( void* destination, const void* source, size_t count );
extern "C" void* memmove( void* destination, const void* source, size_t count );
void* memset( void* destination, unsigned char value, size_t count );
char* strcat( char* destination, char* source );
int strcmp( const char* s1, const char* s2 );
int strncmp( const char* s1, const char* s2, size_t count );
size_t strlen( const char* s );