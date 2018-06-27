
#include "format.h"


void format_chars( output_func_t output, void* ptr, const char* str )
{
	while( *str )
	{
		output( *str++, ptr );
	}
}

void format_number( output_func_t output, void* ptr, unsigned int number )
{
	static char digit_chars[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
	
	int digit = number % 10;
	int remainder = number / 10;

	if( remainder > 0 )
	{
		format_number( output, ptr, remainder );
	}

	output( digit_chars[digit], ptr );
}

void format_number_hex( output_func_t output, void* ptr, unsigned long long number, int width, bool upper )
{
	const char digit_chars_lower[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
	const char digit_chars_upper[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	const char* digit_chars = upper ? digit_chars_upper : digit_chars_lower;

	unsigned int shift = width > 0 ? width * 4 : 64;
	do
	{
		shift -= 4;
		int digit = (number >> shift) & 0xF;
		output( digit_chars[digit], ptr );

		if( shift == 32 )
			output( '`', ptr );
	}
	while( shift != 0 );
}

void format_number_hex( output_func_t output, void* ptr, unsigned int number, int width, bool upper)
{
	const char digit_chars_lower[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
	const char digit_chars_upper[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	const char* digit_chars = upper ? digit_chars_upper : digit_chars_lower;

	unsigned int shift = width > 0 ? width * 4 : 32;
	do
	{
		shift -= 4;
		int digit = (number >> shift) & 0xF;
		output( digit_chars[digit], ptr );
	}
	while( shift != 0 );
}

int isdigit( char ch )
{
	return ch >= '0' && ch <= '9';
}

bool isupper(char ch)
{
	return (ch >= 'A') && (ch <= 'Z');
}
  
void format_string_varg( output_func_t output, void* ptr, const char* format, va_list ap )
{	
	while( *format )
	{
		if( *format == '%' )
		{
			format++;

			//bool leading_zeros = false;
			if (bool leading_zeros = *format == '0')
			{
				leading_zeros = true;
				++format;
			}

			int width = 0;
			if( isdigit( *format ) )
			{
				width = (*format++ - '0');
			}

			bool long_mode = false;
			if( *format == 'l' )
			{
				long_mode = true;
				format++;
			}
			
			switch( *format )
			{
			case 'd':
				format_number( output, ptr, va_arg( ap, unsigned int ) );
				break;
			case 's':
				format_chars( output, ptr, va_arg( ap, char* ) );
				break;
			case 'X':
			case 'x':
			case 'p':
			case 'P':
				if( long_mode )
				{
					format_number_hex( output, ptr, va_arg( ap, unsigned long long ), width, isupper(*format) );
				}
				else
				{
					format_number_hex( output, ptr, va_arg( ap, unsigned int ), width, isupper(*format));
				}
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
