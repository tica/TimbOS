
#ifndef _IO_H_
#define _IO_H_

#include "system.h"

EXTERN_C
void outportb( unsigned short port, unsigned char value );  // Output a byte to a port

EXTERN_C
void outportw( unsigned short port, unsigned short value );  // Output a word to a port

EXTERN_C
unsigned char inportb( unsigned short port );

#endif
