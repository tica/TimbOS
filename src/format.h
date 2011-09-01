
#ifndef _FORMAT_H_
#define _FORMAT_H_

#ifdef __cplusplus
extern "C"
{
#endif

typedef char* va_list;

#define __va_argsiz(t)	\
	(((sizeof(t) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))
	
#define va_start(ap, pN)	\
	((ap) = ((va_list) (&pN) + __va_argsiz(pN)))

#define va_end(ap)	((void)0)

#define va_arg(ap, t)					\
	 (((ap) = (ap) + __va_argsiz(t)),		\
	  *((t*) (void*) ((ap) - __va_argsiz(t))))

typedef void (*output_func_t)( char ch, void* ptr );

void format_string( output_func_t output, void* ptr, const char* format, ... );
void format_string_varg( output_func_t output, void* ptr, const char* format, va_list ap );

#ifdef __cplusplus
}
#endif

#endif //  _FORMAT_H_