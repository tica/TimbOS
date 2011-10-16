
#include "system.h"
#include "video.h"
#include "lib/memory.h"
#include "console.h"
#include "debug.h"
#include "multiboot.h"
#include "processor.h"
#include "elf.h"
#include "paging.h"
#include "irq.h"
#include "kb.h"
#include "idt.h"
#include "isr.h"
#include "scheduler.h"
#include "gdt.h"
#include "mm.h"

struct CONSOLE console;

extern "C" void gdt_install( void );



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

template<char ch>
void taskX( void )
{
	static int skip = 0;

	while( true )
	{
		if( ++skip == 100 )
		{
			skip = 0;
			console.printf( "%c", ch );
		}
	}
}

/*
static uint8_t __ATTRIBUTE_PAGEALIGN__ stackA[4096];
static uint8_t __ATTRIBUTE_PAGEALIGN__ stackAk[4096];
static uint8_t __ATTRIBUTE_PAGEALIGN__ stackB[4096];
static uint8_t __ATTRIBUTE_PAGEALIGN__ stackBk[4096];
*/
static uint8_t __ATTRIBUTE_PAGEALIGN__ stackC[4096];
static uint8_t __ATTRIBUTE_PAGEALIGN__ stackCk[4096];

void floppy_test();
cpu_state* floppy_irq( cpu_state* regs )
{
	console.printf( "floppy irq!\n" );
	return regs;
}

void parse_command_line( multiboot_info* mbt )
{
	if( mbt->flags & MULTIBOOT_INFO_BOOT_LOADER_NAME )
	{
		console.printf( "MultiBoot boot loader detected: %s\n", mbt->boot_loader_name );
	}

	if( mbt->flags & MULTIBOOT_INFO_CMDLINE )
	{
		console.printf( "MultiBoot command line: '%s'\n", mbt->cmdline );
	}
}

extern "C" void kmain( multiboot_info* mbt_info, unsigned int magic )
{
	TRACE2( mbt_info, magic );
	console.printf( "******************************************************************\n" );
	console.printf( "************************* This is TimbOS *************************\n" );
	console.printf( "******************************************************************\n" );

	parse_command_line( mbt_info );
	mm::init( mbt_info );

	check_cpu();
	gdt::init();
    idt::init();
    isr::init();
    irq::init();
	keyboard::init();

	//mm::paging::paging_build_kernel_table( &virtual_mbt->u.elf_sec, 0xC0000000 );

	mm::paging::paging_test();

	for( int i = 0; i < 5; ++i )
		console.printf( "pmem_alloc() = %x, pmem_alloc_dma() = %x\n", mm::alloc_pages(1,1), mm::alloc_pages_dma( 32, 32 ) );
	
	console.printf( "idle\n" );

	scheduler::new_task( stackC, stackCk, (void*)floppy_test );
	irq::install_handler( 0, scheduler::next );
	irq::install_handler( 6, floppy_irq );

	debug_bochs_printf( "\n\nCrashing?\n\n" );
	interrupts_enable();

	// stay!
	while( 1 )
	{
	}

	taskX<'A'>();
	taskX<'B'>();
	taskX<'C'>();
}
