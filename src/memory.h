
#include "system.h"

#ifdef __cplusplus
extern "C" {
#endif

void* memcpy( void* destination, const void* source, size_t count );
void* memset( void* destination, unsigned char value, size_t count );
char* strcat( char* destination, char* source );

#ifdef __cplusplus
}
#endif
