
#include "system.h"
#include "video.h"
#include <memory.h>
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
#include "kheap.h"
#include "drv.h"
#include "devmanager.h"
#include "diskcache.h"
#include "FAT12FileSystem.h"

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

void floppy_init();
void floppy_test();

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


//ASCII magic don't touch!
void sparta()
{
	//first line
	console.printf( "\x0c9" );
	console.printn( '\x0cd', 30 );
	console.printf( "\x0cb" );
	console.printn( '\x0cd', 16 );
	console.printf( "\x0cb" );
	console.printn( '\x0cd', 30 );
	console.printf( "\x0bb" );

	//second line
	console.printf( "\x0ba\x0b0\x0b0\x0b0\x0b1\x0b1\x0b1\x0b2\x0b2\x0b2" );
	console.printn( '\x0db', 12 );
	console.printf( "\x0b2\x0b2\x0b2\x0b1\x0b1\x0b1\x0b0\x0b0\x0b0\x0ba" );
	console.printf( "\x0afThis is TimbOS\x0ae" );
	console.printf( "\x0ba\x0b0\x0b0\x0b0\x0b1\x0b1\x0b1\x0b2\x0b2\x0b2" );
	console.printn( '\x0db', 12 );
	console.printf( "\x0b2\x0b2\x0b2\x0b1\x0b1\x0b1\x0b0\x0b0\x0b0\x0ba" );

	//third line
	console.printf( "\x0c8" );
	console.printn( '\x0cd', 30 );
	console.printf( "\x0ca" );
	console.printn( '\x0cd', 16 );
	console.printf( "\x0ca" );
	console.printn( '\x0cd', 30 );
	console.printf( "\x0bc" );
}

extern "C" void kmain( multiboot_info* mbt_info, unsigned int magic )
{
	TRACE2( mbt_info, magic );

	sparta();

	parse_command_line( mbt_info );
	mm::init( mbt_info );

	check_cpu();
	gdt::init();
    idt::init();
    isr::init();
    irq::init();
	keyboard::init();

	//mm::paging::paging_build_kernel_table( &virtual_mbt->u.elf_sec, 0xC0000000 );

#if 0
	mm::paging::paging_test();
#endif

#if 0
	for( int i = 0; i < 5; ++i )
		console.printf( "pmem_alloc() = %x, pmem_alloc_dma() = %x\n", mm::alloc_pages(1,1), mm::alloc_pages_dma( 32, 32 ) );
#endif

#if 0
	void* ptrlist[1024];

	memtest:
	for( unsigned int i = 0; i < _countof(ptrlist); ++i )
	{
		size_t sz = 64; //i * 64;
		void* p = mm::heap::alloc( sz );

		debug_bochs_printf( "mm::heap::alloc( %x ) = %x\n", sz, p );

		ptrlist[i] = p;
	}

	for( unsigned int i = 0; i < _countof(ptrlist); ++i )
	{
		void* p = ptrlist[i];
		debug_bochs_printf( "calling mm::heap::free( %x )...\n", p );		
		mm::heap::free( p );
	}

	goto memtest;
#endif

	scheduler::init();
	floppy_init();	

	debug_bochs_printf( "\n\nAbout to enable interrupts... crashing???\n" );
	interrupts_enable();
	debug_bochs_printf( "Not yet!\n\n" );

	drv::DeviceManager devmgr;
	drv::manager::probe_all( devmgr );
	
	console.printf( "idle\n" );

	auto floppy0 = devmgr.floppy0_cache();
	auto bootSect = floppy0->cache( 0, 1 );
	bootSect->lock();

	auto fs = fs::FAT12FileSystem::tryCreate( floppy0, bootSect );
	
	auto root = fs->root_directory();

	fs::IFile* file;
	while( root->next( &file ) )
	{
		debug_bochs_printf( "root->next returned true\n" );
	}
	debug_bochs_printf( "root->next returned false\n" );

	bootSect->unlock();


	//scheduler::new_task( mm::alloc_pages(), (void*)floppy_test );
	//scheduler::new_task( mm::alloc_pages(), (void*)taskX<'A'> );
	//scheduler::new_task( mm::alloc_pages(), (void*)taskX<'B'> );

	// stay!
	while( 1 )
	{
	}

	taskX<'A'>();
	taskX<'B'>();
	taskX<'C'>();
}
