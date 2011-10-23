
#include "system.h"
#include "idt.h"

#include "processor.h"
#include <memory.h>
#include "debug.h"

/* Defines an IDT entry */
struct idt_entry
{
    unsigned short base_lo;
    unsigned short sel;
    unsigned char always0;
    unsigned char flags;
    unsigned short base_hi;
} __ATTRIBUTE_PACKED__;

struct idt_ptr
{
    unsigned short limit;
    struct idt_entry* base;
} __ATTRIBUTE_PACKED__;

struct idt_entry	s_idt[256] = {};

extern "C" void idt_load( struct idt_ptr* );

/* Installs the IDT */
void idt::init()
{
	debug_bochs_printf( "idt::init..." );

	struct idt_ptr idtp =
	{
	    (sizeof s_idt) - 1,
		s_idt
	};

	debug_bochs_printf( "Installing IDT from ptr %x, base = %x\n", &idtp, idtp.base );
    idt_load( &idtp );
}

void idt::set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags)
{
    /* The interrupt routine's base address */
    s_idt[num].base_lo = (base & 0xFFFF);
    s_idt[num].base_hi = (base >> 16) & 0xFFFF;

    /* The segment or 'selector' that this IDT entry will use
    *  is set here, along with any access flags */
    s_idt[num].sel = sel;
    s_idt[num].always0 = 0;
    s_idt[num].flags = flags;
}
