
#pragma once

#include "stddef.h"

struct div_t
{
	int quot;
	int rem;
};

struct ldiv_t
{
	long int quot;
	long int rem;
};

double atof(const char *string);
int atoi(const char *string);
long atol(const char *string);
double strtod(const char *string, char **tailptr);
float strtof(const char *string, char **tailptr);
long double strtold(const char *string, char **tailptr);
long int strtol(const char *nptr, char **endptr, int base);
long long int strtoll(const char *nptr, char **endptr, int base);
unsigned long int strtoul(const char *nptr, char **endptr, int base);
unsigned long long int strtoull(const char *nptr, char **endptr, int base);
int rand(void);
void srand(unsigned int seed);
void * calloc(size_t count, size_t eltsize);
void free(void *ptr);
void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
extern "C" void abort(void);
int atexit(void(*function)(void));
void exit(int status);
void _Exit(int status);
char *getenv(const char *name);
int system(const char *command);
void *bsearch(const void *key, const void *base, size_t nmemb, size_t size, int(*compar)(const void *, const void *));
void qsort(void *base, size_t nmemb, size_t size, int(*compar)(const void *, const void *));
int abs(int j);
long labs(long x);
div_t div(int numerator, int denominator);
ldiv_t ldiv(long int numerator, long int denominator);
int mblen(const char *s, size_t n);
int mbtowc(wchar_t *pwc, const char *s, size_t n);
int wctomb(char *s, wchar_t wc);
size_t mbstowcs(wchar_t *dest, const char *src, size_t n);
size_t wcstombs(char *dest, const wchar_t *src, size_t n);
