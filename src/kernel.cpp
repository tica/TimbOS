
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

const unsigned int TASK_COUNT = 2;
static struct cpu_state* s_tasks[TASK_COUNT] = {};
static unsigned int s_current_task = (unsigned)-1;

struct cpu_state* timer_handler(struct cpu_state* regs)
{
	/*
	static int count;
	if( ++count == 100 )
	{
		console.printf( "1" );
		count = 0;
	}*/

	if( s_current_task < TASK_COUNT )
	{
		s_tasks[s_current_task] = regs;
	}

	s_current_task = (s_current_task + 1) % TASK_COUNT;

	return s_tasks[s_current_task];
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

void taskA( void )
{
	static int a_skip = 0;

	while( true )
		if( ++a_skip == 1000 )
		{
			a_skip = 0;
			console.printf( "A" );
		}
}

void taskB( void )
{
	static int b_skip = 0;

	while( true )
		if( ++b_skip == 1000 )
		{
			b_skip = 0;
			console.printf( "B" );
		}
}

struct cpu_state* init_task( void* stack, void* entry )
{
	struct cpu_state* regs = reinterpret_cast<struct cpu_state*>( ((uint8_t*)stack) + 4096 - sizeof(struct cpu_state) );
	memset( regs, 0, sizeof(regs) );

	regs->eip = (unsigned int)entry;
	regs->cs = 0x08;
	regs->ds = 0x10;
	regs->es = 0x10;
	regs->fs = 0x10;
	regs->gs = 0x10;
	regs->eflags = 0x0202;

	return regs;
}

static uint8_t __ATTRIBUTE_PAGEALIGN__ stackA[4096];
static uint8_t __ATTRIBUTE_PAGEALIGN__ stackB[4096];

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
	}

	if( virtual_mbt->flags & (1 << 5) )
	{
		console.printf( "Found ELF section table @ %x, total entries = %d\n", virtual_mbt->u.elf_sec.addr, virtual_mbt->u.elf_sec.num );
		virtual_mbt->u.elf_sec.addr = KernelPageDirectory.physical_to_virtual( virtual_mbt->u.elf_sec.addr );
	}

	pmem_init( virtual_mbt );
	paging_build_kernel_table( &virtual_mbt->u.elf_sec, 0xC0000000 );

	debug_bochs_printf( "\n\nCrashing?\n\n" );
	interrupts_enable();

	paging_test();

	console.printf( "idle\n" );

	s_tasks[0] = init_task( stackA, (void*)taskA );
	s_tasks[1] = init_task( stackB, (void*)taskB );
	irq::install_handler( 0, timer_handler );

	// stay!
	while( 1 )
	{
	}
}
