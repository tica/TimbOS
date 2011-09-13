
#include "system.h"
#include "video.h"
#include "memory.h"
#include "console.h"
#include "debug.h"
#include "multiboot.h"
#include "processor.h"
#include "elf.h"
#include "paging.h"
#include "pmem.h"
#include "irq.h"
#include "kb.h"
#include "idt.h"
#include "isr.h"

struct CONSOLE console;

extern "C" void gdt_install( void );
extern "C" void test_test( void );

void timer_handler(struct regs*)
{
	static int count;

	if( ++count == 100 )
	{
		console.printf( "1" );
		count = 0;
	}
}


void check_cpu(void)
{
	uint32_t cpu_name_buffer[4] = {};
	__asm__ __volatile__ ("cpuid" : "=b"(cpu_name_buffer[0]), "=d"(cpu_name_buffer[1]), "=c"(cpu_name_buffer[2]) : "a"(0) : );	
	console.printf("CPU: %s ", cpu_name_buffer);

	unsigned r1, r2;
    __asm__ __volatile__ ("cpuid" : "=a"(r1), "=d"(r2) : "a"(1) : "ecx");
	console.printf("Family %d Model %d Stepping %d\n", (r1 & 0xF00) >> 8, (r1 & 0xF0) >> 4, (r1 & 0xF) );

    console.printf("\n");
}


extern "C" void kmain( uintptr_t physical_multiboot_info, unsigned int magic )
{
	TRACE2( physical_multiboot_info, magic );

	paging_init();

	console.printf( "******************************************************************\n" );
	console.printf( "************************* This is TimbOS *************************\n" );
	console.printf( "******************************************************************\n" );
	check_cpu();

	gdt_install();
    idt::init();
    isr::init();
    irq::init();
	keyboard::init();

	irq::install_handler( 0, timer_handler );

	multiboot_info_t* virtual_mbt = (multiboot_info_t*)KernelPageDirectory.physical_to_virtual( physical_multiboot_info );

	if( virtual_mbt->flags & (1 << 9) )
	{
		virtual_mbt->boot_loader_name = KernelPageDirectory.physical_to_virtual( virtual_mbt->boot_loader_name );

		console.printf( "MultiBoot boot loader detected: %s\n", virtual_mbt->boot_loader_name );
	}

	if( virtual_mbt->flags & (1 << 2) )
	{
		virtual_mbt->cmdline = KernelPageDirectory.physical_to_virtual(virtual_mbt->cmdline);

		console.printf( "MultiBoot command line: '%s'\n", virtual_mbt->cmdline );
	}

	if( virtual_mbt->flags & (1 << 3) )
	{
		console.printf( "GRUB module list found!\n" );
	}

	if( virtual_mbt->flags & (1 << 0) )
	{
		console.printf( "Memory (Lower/Upper): %d KiB/%d KiB\n", virtual_mbt->mem_lower, virtual_mbt->mem_upper );
	}
	else
	{
		console.printf( "WARNING: MultiBoot header did not specify memory\n" );
	}

	if( virtual_mbt->flags & (1 << 6) )
	{
		console.printf( "Found memory map...\n" );

		virtual_mbt->mmap_addr = KernelPageDirectory.physical_to_virtual( virtual_mbt->mmap_addr );
		pmem_init( virtual_mbt );
	}

	if( virtual_mbt->flags & (1 << 5) )
	{
		virtual_mbt->u.elf_sec.addr = KernelPageDirectory.physical_to_virtual( virtual_mbt->u.elf_sec.addr );

		console.printf( "Found ELF section table @ %x, total entries = %d\n", virtual_mbt->u.elf_sec.addr, virtual_mbt->u.elf_sec.num );

		paging_build_kernel_table( &virtual_mbt->u.elf_sec, 0xC0000000 );
	}

	debug_bochs_printf( "\n\nCrashing?\n\n" );
	interrupts_enable();

	paging_test();

	console.printf( "idle" );

	// stay!
	while( 1 )
	{
	}
}
