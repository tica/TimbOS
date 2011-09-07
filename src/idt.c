
#include "system.h"
#include "processor.h"
#include "memory.h"
//#include "paging.h"
#include "debug.h"

/* Defines an IDT entry */
struct idt_entry
{
    unsigned short base_lo;
    unsigned short sel;
    unsigned char always0;
    unsigned char flags;
    unsigned short base_hi;
} __attribute__((packed));

struct idt_ptr
{
    unsigned short limit;
    struct idt_entry* base;
} __attribute__((packed));

struct idt_entry	s_idt[256];
struct idt_ptr		g_idtp;


void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags)
{
	//base = virtual_to_physical( base );

    /* The interrupt routine's base address */
    s_idt[num].base_lo = (base & 0xFFFF);
    s_idt[num].base_hi = (base >> 16) & 0xFFFF;

    /* The segment or 'selector' that this IDT entry will use
    *  is set here, along with any access flags */
    s_idt[num].sel = sel;
    s_idt[num].always0 = 0;
    s_idt[num].flags = flags;

	//debug_bochs_printf( "sidt[%x]: base_lo=%x base_hi=%x \n", num, s_idt[num].base_lo, s_idt[num].base_hi );
}

extern void idt_load( void );
extern void idt_store( void );

/* Installs the IDT */
void idt_install( void )
{
	debug_bochs_printf( "idt_install... " );

	g_idtp.limit = 0;
	g_idtp.base = 0;

    /* Sets the special IDT pointer up, just like in 'gdt.c' */
    g_idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
    //g_idtp.base = (struct idt_entry*)virtual_to_physical( (size_t)s_idt );
	g_idtp.base = s_idt;

	debug_bochs_printf( "Installing IDT from ptr %x, base = %x\n", &g_idtp, g_idtp.base );

    /* Clear out the entire IDT, initializing it to zeros */
    memset( &s_idt, 0, sizeof(struct idt_entry) * 256 );

    /* Add any new ISRs to the IDT here using idt_set_gate */

    /* Points the processor's internal register to the new IDT */

	debug_bochs_printf( "idt_load()?\n" );
    idt_load();
	debug_bochs_printf( "returned!\n" );

	/*
	g_idtp.limit = 0;
	g_idtp.base = 0;
	debug_bochs_printf( "IDT Ptr: base = %x, limit = %x\n", g_idtp.base, g_idtp.limit );
	debug_bochs_printf( "idt_store()?\n" );
	idt_store();
	debug_bochs_printf( "IDT Ptr: base = %x, limit = %x\n", g_idtp.base, g_idtp.limit );
	*/

	debug_bochs_printf( "done\n" );
}

