
#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <stddef.h>
#include <stdint.h>

struct cpu_state
{
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;    
};

#ifdef __cplusplus
#define EXTERN_C		extern "C"
#define EXTERN_C_BEGIN	extern "C" {
#define EXTERN_C_END	}
#else
#define EXTERN_C 
#define EXTERN_C_BEGIN
#define EXTERN_C_END
#endif

#ifndef _MSC_VER
#define __ATTRIBUTE_PACKED__		__attribute__((packed))
#define __ATTRIBUTE_PAGEALIGN__		__attribute__((aligned(4096)))
#else // ifdef _MSC_VER
#define __ATTRIBUTE_PACKED__
#define __ATTRIBUTE_PAGEALIGN__
#endif

#define _countof(array)	(sizeof(array)/sizeof(array[0]))

EXTERN_C void irq_install_handler(int irq, void (*handler)(struct cpu_state *r));


#endif // _SYSTEM_H_
