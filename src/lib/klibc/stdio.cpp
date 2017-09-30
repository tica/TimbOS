
#include "stdio.h"

#define restrict

int snprintf(char* restrict s, size_t n, const char* restrict format, ...)
{
	(void)s;
	(void)n;
	(void)format;

	return 0;
}

int fprintf(FILE* restrict stream, const char* restrict format, ...)
{
	(void)stream;
	(void)format;

	return 0;
}