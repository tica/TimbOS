
#include "system.h"
#include "irq.h"

#include "idt.h"
#include "io.h"
#include "debug.h"

/* These are own ISRs that point to our special IRQ handler
*  instead of the regular 'fault_handler' function */
extern "C"
{
	void irq0();
	void irq1();
	void irq2();
	void irq3();
	void irq4();
	void irq5();
	void irq6();
	void irq7();
	void irq8();
	void irq9();
	void irq10();
	void irq11();
	void irq12();
	void irq13();
	void irq14();
	void irq15();
}

/* This array is actually an array of function pointers. We use
*  this to handle custom IRQ handlers for a given IRQ */
static irq::handler_proc irq_routines[16] = {};

/* This installs a custom IRQ handler for the given IRQ */
void irq::install_handler( int irq, handler_proc handler )
{
    irq_routines[irq] = handler;
}

/* This clears the handler for a given IRQ */
void irq::uninstall_handler( int irq )
{
    irq_routines[irq] = 0;
}

/* Normally, IRQs 0 to 7 are mapped to entries 8 to 15. This
*  is a problem in protected mode, because IDT entry 8 is a
*  Double Fault! Without remapping, every time IRQ0 fires,
*  you get a Double Fault Exception, which is NOT actually
*  what's happening. We send commands to the Programmable
*  Interrupt Controller (PICs - also called the 8259's) in
*  order to make IRQ0 to 15 be remapped to IDT entries 32 to
*  47 */
void irq_remap(void)
{
    outportb(0x20, 0x11); // Init
    outportb(0xA0, 0x11); // Init
    outportb(0x21, 0x20); // Map to 32+
    outportb(0xA1, 0x28); // Map to 38+
    outportb(0x21, 0x04); // Master: Slave port is 2 (bit-2)
    outportb(0xA1, 0x02); // Slave: Master port is 2 (number-2)
    outportb(0x21, 0x01); // Enable waiting
    outportb(0xA1, 0x01); // Enable waiting
    //outportb(0x21, 0x01); // mask timer 
    //outportb(0xA1, 0x00); 

	outportb(0xA0, 0x20); // EOI ?
	outportb(0x20, 0x20);
}

/* We first remap the interrupt controllers, and then we install
*  the appropriate ISRs to the correct entries in the IDT. This
*  is just like installing the exception handlers */
void irq::init()
{
	debug_bochs_printf( "irq_install... " );

    irq_remap();

	int sel = 0x08;

    idt::set_gate(32, (unsigned)irq0, sel, 0x8E);
    idt::set_gate(33, (unsigned)irq1, sel, 0x8E);
    idt::set_gate(34, (unsigned)irq2, sel, 0x8E);
    idt::set_gate(35, (unsigned)irq3, sel, 0x8E);
    idt::set_gate(36, (unsigned)irq4, sel, 0x8E);
    idt::set_gate(37, (unsigned)irq5, sel, 0x8E);
    idt::set_gate(38, (unsigned)irq6, sel, 0x8E);
    idt::set_gate(39, (unsigned)irq7, sel, 0x8E);

    idt::set_gate(40, (unsigned)irq8, sel, 0x8E);
    idt::set_gate(41, (unsigned)irq9, sel, 0x8E);
    idt::set_gate(42, (unsigned)irq10, sel, 0x8E);
    idt::set_gate(43, (unsigned)irq11, sel, 0x8E);
    idt::set_gate(44, (unsigned)irq12, sel, 0x8E);
    idt::set_gate(45, (unsigned)irq13, sel, 0x8E);
    idt::set_gate(46, (unsigned)irq14, sel, 0x8E);
    idt::set_gate(47, (unsigned)irq15, sel, 0x8E);

	debug_bochs_printf( "done\n" );
}

/* Each of the IRQ ISRs point to this function, rather than
*  the 'fault_handler' in 'isrs.c'. The IRQ Controllers need
*  to be told when you are done servicing them, so you need
*  to send them an "End of Interrupt" command (0x20). There
*  are two 8259 chips: The first exists at 0x20, the second
*  exists at 0xA0. If the second controller (an IRQ from 8 to
*  15) gets an interrupt, you need to acknowledge the
*  interrupt at BOTH controllers, otherwise, you only send
*  an EOI command to the first controller. If you don't send
*  an EOI, you won't raise any more IRQs */
extern "C" struct cpu_state* irq_handler( struct cpu_state *r )
{
    /* Find out if we have a custom handler to run for this
    *  IRQ, and then finally, run it */
    irq::handler_proc handler = irq_routines[r->int_no - 32];
    if (handler)
    {
        r = handler(r);
    }
	else
	{
		debug_bochs_printf( "No handler registered for IRQ %x\n", r->int_no - 32 );
	}

    /* If the IDT entry that was invoked was greater than 40
    *  (meaning IRQ8 - 15), then we need to send an EOI to
    *  the slave controller */
    if (r->int_no >= 40)
    {
        outportb(0xA0, 0x20);
    }

    /* In either case, we need to send an EOI to the master
    *  interrupt controller too */
    outportb(0x20, 0x20);

	return r;
}
