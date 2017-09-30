
#pragma once

#include <stddef.h>

void* memcpy( void* destination, const void* source, size_t count );
extern "C" void* memmove( void* destination, const void* source, size_t count );
void* memset( void* destination, unsigned char value, size_t count );
char* strcat( char* destination, char* source );
int strcmp( const char* s1, const char* s2 );
int strncmp( const char* s1, const char* s2, size_t count );
size_t strlen( const char* s );

char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
char *strncat(char *dest, const char *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
int strcoll(const char *s1, const char *s2);
size_t strxfrm(char *dest, const char *src, size_t n);
void *memchr(const void *s, int c, size_t n);
char *strchr(const char *s, int c);
size_t strcspn(const char *s, const char *reject);
char *strpbrk(const char *s, const char *accept);
char *strrchr(const char *s, int c);
size_t strspn(const char *s, const char *accept);
char *strstr(const char *haystack, const char *needle);
char *strtok(char *str, const char *delim);
char *strerror(int errnum);