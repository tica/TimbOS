
#include "format.h"


void format_chars( output_func_t output, void* ptr, const char* str )
{
	while( *str )
	{
		output( *str++, ptr );
	}
}

void format_number_hex( output_func_t output, void* ptr, unsigned int number )
{
	format_chars( output, ptr, "0x" );
	char digit_chars[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

	unsigned int shift = 32;
	do
	{
		shift -= 4;
		int digit = (number >> shift) & 0xF;
		output( digit_chars[digit], ptr );
	}
	while( shift != 0 );
}
  
void format_string_varg( output_func_t output, void* ptr, const char* format, va_list ap )
{	
	while( *format )
	{
		if( *format == '%' )
		{
			format++;
			
			switch( *format )
			{
			case 's':
				format_chars( output, ptr, va_arg( ap, char* ) );
				break;
			case 'x':
			case 'p':
				format_number_hex( output, ptr, va_arg( ap, unsigned int ) );
				break;
			case 'c':
				output( va_arg( ap, char ), ptr );
				break;
			default:
				output( '%', ptr );
				output( *format, ptr );
				break;
			}
			
			format++;
		}
		else
		{
			output( *format++, ptr );
		}
	}
}
	  
void format_string( output_func_t output, void* ptr, const char* format, ... )
{
	va_list ap;
	va_start( ap, format );

	format_string_varg( output, ptr, format, ap );

	va_end( ap );
}