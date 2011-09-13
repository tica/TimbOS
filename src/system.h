
#ifndef _SYSTEM_H_
#define _SYSTEM_H_

typedef unsigned long size_t;
typedef unsigned long uintptr_t;

typedef unsigned int uint32_t;
typedef int int32_t;

struct regs
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
#endif

EXTERN_C void irq_install_handler(int irq, void (*handler)(struct regs *r));


#endif // _SYSTEM_H_
