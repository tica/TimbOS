
#include "system.h"
#include "video.h"
#include "memory.h"
#include "console.h"
#include "debug.h"
#include "multiboot.h"
#include "processor.h"
#include "elf.h"
#include "paging.h"
#include "physmem.h"

struct CONSOLE console;

extern "C" void gdt_install( void );
extern "C" void idt_install( void );
extern "C" void isrs_install( void );
extern "C" void irq_install( void );
extern "C" void keyboard_install( void );
extern "C" void test_test( void );

extern "C" void irq_install_handler(int irq, void (*handler)(struct regs *r));

void timer_handler(struct regs *r)
{
	static int count;
	r = r;

	if( ++count == 100*3 )
	{
		console.printf( "3 seconds\n" );
		debug_bochs_printf( "3 seconds\n" );
		count = 0;
	}
}

extern "C" void print_char( char ch )
{
	console.printf( "%c", ch );
}

extern "C" void kmain( multiboot_info_t* phyiscal_mbt, unsigned int magic )
{
	TRACE2( phyiscal_mbt, magic );

	paging_init();

	console.printf( "This is TimbOS\n" );
	console.printf( "mbt = %x, magic = %x\n", phyiscal_mbt, magic );

	/*
	console.printf( "FLAGS = %x\n", read_flags() );
	console.printf( "CR0 = %x\n", read_cr0() );
	console.printf( "CR3 = %x\n", read_cr3() );
	console.printf( "BootPageDirectory = %x\n", get_boot_pagedir() );
	*/

	/*
	int x = 0;
	console.printf( "&x (stack) = %x\n", &x );
	console.printf( "&console (.data) = %x\n", &console );
	console.printf( "physical( &x (stack) ) = %x\n", virtual_to_physical( &x ) );
	console.printf( "physical( &console (.data) ) = %x\n", virtual_to_physical( &console ) );
	*/

	gdt_install();
    idt_install();
    isrs_install();
    irq_install();
	keyboard_install();
	irq_install_handler( 0, timer_handler );	

	multiboot_info_t* virtual_mbt = physical_to_virtual( phyiscal_mbt );

	debug_bochs_printf( "phyiscal_mbt->mem_lower = %x\n", virtual_mbt->mem_lower * 1024 );
	debug_bochs_printf( "phyiscal_mbt->mem_upper = %x\n", virtual_mbt->mem_upper * 1024 );

	virtual_mbt->u.elf_sec.addr = physical_to_virtual( virtual_mbt->u.elf_sec.addr );
	virtual_mbt->mmap_addr = physical_to_virtual( virtual_mbt->mmap_addr );
	physmem_init_from_mbt( virtual_mbt );


	paging_build_kernel_table( &virtual_mbt->u.elf_sec, 0xC0000000 );


	/*
	virtual_mbt = physical_to_virtual( phyiscal_mbt );
	virtual_mbt->mmap_addr = physical_to_virtual( virtual_mbt->mmap_addr );
	*/


	//console.printf( "cr0 = %x", read_cr0() );

	debug_bochs_printf( "\n\nCrashing?\n\n" );
	interrupts_enable();

	paging_test();

	console.printf( "idle" );
	// stay!
	while( 1 )
	{
	}
}