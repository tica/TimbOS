
#pragma once

#include "stddef.h"

typedef char* va_list;

struct FILE
{
	void* unused;
};

typedef long fpos_t;

#define EOF    (-1)

#define stderr	((FILE*)0)

int fclose(FILE* stream);
int fflush(FILE* stream);
void setbuf(FILE* stream, char* buf);
int setvbuf(FILE* stream, char* buf, int mode, size_t size);
int fprintf(FILE* stream, const char* format, ...);
int fscanf(FILE* stream, const char * format, ...);
int snprintf(char* s, size_t n, const char* format, ...);    // C99
int sprintf(char* s, const char* format, ...);
int sscanf(const char* s, const char* format, ...);
int vfprintf(FILE* stream, const char* format, va_list arg);
int vfscanf(FILE* stream, const char* format, va_list arg);  // C99
int vsnprintf(char* s, size_t n, const char* format, va_list arg); // C99	
int vsprintf(char* s, const char* format, va_list arg);
int vsscanf(const char* s, const char* format, va_list arg); // C99
int fgetc(FILE* stream);
char* fgets(char* s, int n, FILE* stream);
int fputc(int c, FILE* stream);
int fputs(const char* s, FILE* stream);
int getc(FILE* stream);
int putc(int c, FILE* stream);
int ungetc(int c, FILE* stream);
size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream);
size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream);
int fgetpos(FILE* stream, fpos_t* pos);
int fseek(FILE* stream, long offset, int whence);
int fsetpos(FILE* stream, const fpos_t* pos);
long ftell(FILE* stream);
void rewind(FILE* stream);
void clearerr(FILE* stream);
int feof(FILE* stream);
int ferror(FILE* stream);
void perror(const char* s);

