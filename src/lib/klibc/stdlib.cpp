
#include "stdlib.h"

extern "C" void abort()
{
	while (1)
		;
}

long labs(long x)
{
	if (x < 0)
		return -x;

	return x;
}

div_t div(int numerator, int denominator)
{
	return { numerator / denominator, numerator % denominator };
}

ldiv_t ldiv(long int numerator, long int denominator)
{
	return { numerator / denominator, numerator % denominator };
}

long int strtol(const char *nptr, char **endptr, int base)
{
	(void)nptr;
	(void)endptr;
	(void)base;

	return 0;
}
long long int strtoll(const char *nptr, char **endptr, int base)
{
	(void)nptr;
	(void)endptr;
	(void)base;

	return 0;
}
unsigned long int strtoul(const char *nptr, char **endptr, int base)
{
	(void)nptr;
	(void)endptr;
	(void)base;

	return 0;
}
unsigned long long int strtoull(const char *nptr, char **endptr, int base)
{
	(void)nptr;
	(void)endptr;
	(void)base;

	return 0;
}

double strtod(const char *string, char **tailptr)
{
	(void)string;
	(void)tailptr;

	return 0;
}
float strtof(const char *string, char **tailptr)
{
	(void)string;
	(void)tailptr;

	return 0;
}
long double strtold(const char *string, char **tailptr)
{
	(void)string;
	(void)tailptr;

	return 0;
}