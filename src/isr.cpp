
#include "system.h"
#include "isr.h"

#include "idt.h"
#include "debug.h"
#include "processor.h"

/* These are function prototypes for all of the exception
*  handlers: The first 32 entries in the IDT are reserved
*  by Intel, and are designed to service exceptions! */
extern "C"
{
	void isr0();
	void isr1();
	void isr2();
	void isr3();
	void isr4();
	void isr5();
	void isr6();
	void isr7();
	void isr8();
	void isr9();
	void isr10();
	void isr11();
	void isr12();
	void isr13();
	void isr14();
	void isr15();
	void isr16();
	void isr17();
	void isr18();
	void isr19();
	void isr20();
	void isr21();
	void isr22();
	void isr23();
	void isr24();
	void isr25();
	void isr26();
	void isr27();
	void isr28();
	void isr29();
	void isr30();
	void isr31();
}

/* This is a very repetitive function... it's not hard, it's
*  just annoying. As you can see, we set the first 32 entries
*  in the IDT to the first 32 ISRs. We can't use a for loop
*  for this, because there is no way to get the function names
*  that correspond to that given entry. We set the access
*  flags to 0x8E. This means that the entry is present, is
*  running in ring 0 (kernel level), and has the lower 5 bits
*  set to the required '14', which is represented by 'E' in
*  hex. */
void isr::init()
{
	debug_bochs_printf( "isrs_install... " );

	int sel = 0x08;

    idt::set_gate(0, (unsigned)isr0, sel, 0x8E);
    idt::set_gate(1, (unsigned)isr1, sel, 0x8E);
    idt::set_gate(2, (unsigned)isr2, sel, 0x8E);
    idt::set_gate(3, (unsigned)isr3, sel, 0x8E);
    idt::set_gate(4, (unsigned)isr4, sel, 0x8E);
    idt::set_gate(5, (unsigned)isr5, sel, 0x8E);
    idt::set_gate(6, (unsigned)isr6, sel, 0x8E);
    idt::set_gate(7, (unsigned)isr7, sel, 0x8E);

    idt::set_gate(8, (unsigned)isr8, sel, 0x8E);
    idt::set_gate(9, (unsigned)isr9, sel, 0x8E);
    idt::set_gate(10, (unsigned)isr10, sel, 0x8E);
    idt::set_gate(11, (unsigned)isr11, sel, 0x8E);
    idt::set_gate(12, (unsigned)isr12, sel, 0x8E);
    idt::set_gate(13, (unsigned)isr13, sel, 0x8E);
    idt::set_gate(14, (unsigned)isr14, sel, 0x8E);
    idt::set_gate(15, (unsigned)isr15, sel, 0x8E);

    idt::set_gate(16, (unsigned)isr16, sel, 0x8E);
    idt::set_gate(17, (unsigned)isr17, sel, 0x8E);
    idt::set_gate(18, (unsigned)isr18, sel, 0x8E);
    idt::set_gate(19, (unsigned)isr19, sel, 0x8E);
    idt::set_gate(20, (unsigned)isr20, sel, 0x8E);
    idt::set_gate(21, (unsigned)isr21, sel, 0x8E);
    idt::set_gate(22, (unsigned)isr22, sel, 0x8E);
    idt::set_gate(23, (unsigned)isr23, sel, 0x8E);

    idt::set_gate(24, (unsigned)isr24, sel, 0x8E);
    idt::set_gate(25, (unsigned)isr25, sel, 0x8E);
    idt::set_gate(26, (unsigned)isr26, sel, 0x8E);
    idt::set_gate(27, (unsigned)isr27, sel, 0x8E);
    idt::set_gate(28, (unsigned)isr28, sel, 0x8E);
    idt::set_gate(29, (unsigned)isr29, sel, 0x8E);
    idt::set_gate(30, (unsigned)isr30, sel, 0x8E);
    idt::set_gate(31, (unsigned)isr31, sel, 0x8E);

	debug_bochs_printf( "done\n" );
}

/* This is a simple string array. It contains the message that
*  corresponds to each and every exception. We get the correct
*  message by accessing like:
*  exception_message[interrupt_number] */
const char* exception_messages[] =
{
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

extern "C" bool paging_handle_fault( struct cpu_state* r, uintptr_t virtual_address );

/* All of our Exception handling Interrupt Service Routines will
*  point to this function. This will tell us what exception has
*  happened! Right now, we simply halt the system by hitting an
*  endless loop. All ISRs disable interrupts while they are being
*  serviced as a 'locking' mechanism to prevent an IRQ from
*  happening and messing up kernel data structures */
extern "C" void fault_handler(struct cpu_state *r)
{
	if (r->int_no < 32)
    {
		bool handled = false;		

		switch( r->int_no )
		{
		case 14: // Page fault			
			handled = paging_handle_fault( r, read_cr2() );
			break;
		default:
			debug_bochs_printf( "EXCEPTION!" );
			debug_bochs_printf( " (%x) (err_code = %x)\n", r->int_no, r->err_code );	
			debug_bochs_printf( "%s Exception\n", exception_messages[r->int_no] );
			break;
		}

		if( !handled )
		{
			debug_bochs_printf( "\nRegister dump:\n" );

			/*
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;
	*/
			debug_bochs_printf( "EAX = %x EBX = %x ECX = %x EDX = %x\n", r->eax, r->ebx, r->ecx, r->edx );
			debug_bochs_printf( "ESP = %x EBP = %x ESI = %x EDI = %x\n", r->esp, r->ebp, r->esi, r->edi );
			debug_bochs_printf( "GS  = %x FS  = %x ES  = %x DS  = %x\n", r->gs, r->fs, r->es, r->ds );
			debug_bochs_printf( "EIP = %x CS  = %x EFLAGS = %x SS = %x\n", r->eip, r->cs, r->eflags, r->ss );

			debug_bochs_printf( "System Halted!\n" );


			for (;;);
		}
    }
}
