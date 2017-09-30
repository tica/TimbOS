
#include "wchar.h"

#define restrict

long wcstol(const wchar_t* restrict nptr, wchar_t** restrict endptr, int base)
{
	(void)nptr;
	(void)endptr;
	(void)base;

	return 0;
}
long long wcstoll(const wchar_t* restrict nptr, wchar_t** restrict endptr, int base)
{
	(void)nptr;
	(void)endptr;
	(void)base;

	return 0;
}
unsigned long wcstoul(const wchar_t* restrict nptr, wchar_t** restrict endptr, int base)
{
	(void)nptr;
	(void)endptr;
	(void)base;

	return 0;
}
unsigned long long wcstoull(const wchar_t* restrict nptr, wchar_t** restrict endptr, int base)
{
	(void)nptr;
	(void)endptr;
	(void)base;

	return 0;
}

double wcstod(const wchar_t* restrict nptr, wchar_t** restrict endptr)
{
	(void)nptr;
	(void)endptr;

	return 0;
}
float wcstof(const wchar_t* restrict nptr, wchar_t** restrict endptr)
{
	(void)nptr;
	(void)endptr;

	return 0;
}
long double wcstold(const wchar_t* restrict nptr, wchar_t** restrict endptr)
{
	(void)nptr;
	(void)endptr;

	return 0;
}

int swprintf(wchar_t* restrict s, size_t n, const wchar_t* restrict format, ...)
{
	(void)s;
	(void)n;
	(void)format;

	return 0;
}

wchar_t* wmemcpy(wchar_t* restrict s1, const wchar_t* restrict s2, size_t n)
{
	(void)s1;
	(void)s2;
	(void)n;

	return nullptr;
}
wchar_t* wmemmove(wchar_t* s1, const wchar_t* s2, size_t n)
{
	(void)s1;
	(void)s2;
	(void)n;

	return nullptr;
}
wchar_t* wmemset(wchar_t* s, wchar_t c, size_t n)
{
	(void)s;
	(void)c;
	(void)n;

	return nullptr;
}