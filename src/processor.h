
#ifndef _PROCESSOR_H_
#define _PROCESSOR_H_

#include "system.h"

#ifdef __cplusplus
extern "C"
{
#endif

	unsigned long read_flags( void );
	unsigned long read_cr0( void );
	unsigned long read_cr2( void );
	unsigned long read_cr3( void );
	void set_pagedir( void );

	void interrupts_enable( void );
	void interrupts_disable( void );

#ifdef __cplusplus
}
#endif

#endif  // _PROCESSOR_H_
