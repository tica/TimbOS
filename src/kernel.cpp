
#include "system.h"
#include "video.h"
#include <string.h>
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
#include "syscall.h"
#include "interlocked.h"

#include "test/stl_test.h"

#include <string>
#include <atomic>
#include <array>

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
		if( ++skip == 10000 )
		{
			skip = 0;
			console.printf( "%c", ch );
		}
	}
}

int count_a = 0, count_b = 0;
volatile int global_counter;
uint32_t counting_cs = 0;
uint32_t printing_cs = 0;

template<char ch, int& local>
void counting_test( void )
{
	for( int i = 1; i <= 100'000'000; ++i )
	{
		//interlocked::enter( &counting_cs );

		//atomic_counter += 1;

		//interlocked::leave( &counting_cs );

		/*
		if (ch == 'B')
		{
			panic("moep");
		}		
		*/

		local += 1;
		global_counter += 1;

		if (i % 1000000 == 0)
		{
			interlocked::enter(&printing_cs);
			debug_bochs_printf("Task %c: a = %d b = %d global = %d\n", ch, count_a, count_b, global_counter);
			interlocked::leave(&printing_cs);
		}
	}

	{
		interlocked::enter(&printing_cs);
		debug_bochs_printf("DONE %c: a = %d b = %d global = %d\n", ch, count_a, count_b, global_counter);
		interlocked::leave(&printing_cs);
	}

	while( 1 );
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
	console.printf( "\n" );
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
	syscall::init();

	floppy_init();	

	debug_bochs_printf( "\n\nAbout to enable interrupts... crashing???\n" );
	interrupts_enable();
	debug_bochs_printf( "Not yet!\n\n" );

	drv::DeviceManager devmgr;
	drv::manager::probe_all( devmgr );

	/*
	uint64_t a = 0x0000001100000001;
	console.printf( "&a = %p\n", &a );
	console.printf( "a = %lx\n", a );
	uint64_t b = interlocked::compare_exchange( &a, 0x0000001100000001, 0x0000002200000002 );	
	console.printf( "a = %lx b = %lx\n", a, b );
	*/

	console.printf( "idle\n" );	

	auto floppy0 = devmgr.get("/floppy/0");

	std::array<char, 512> bootSectorData;
	floppy0->read(0, 1, &bootSectorData);

	auto fs = fs::FAT12FileSystem::tryCreate( floppy0, bootSectorData.data());
	
	auto root = fs->root_directory();

	fs::IFileSystemEntry* file;
	//char file_name[20] = {};
	while( root->next( &file ) )
	{
		debug_bochs_printf( "root->next returned true\n" );
		debug_bochs_printf( "file = %p\n", file );		

		//file->get_name( file_name, sizeof(file_name) );
		//console.printf("file = %s\n", file_name);
	}
	debug_bochs_printf( "root->next returned false\n" );

	//test_stl();

	//scheduler::new_task( mm::alloc_pages(), (void*)floppy_test );
	//scheduler::new_task( mm::alloc_pages(), (void*)taskX<'A'> );
	//scheduler::new_task( mm::alloc_pages(), (void*)taskX<'B'> );
	
	//scheduler::new_task( mm::alloc_pages(), (void*)counting_test<'A', count_a> );
	//scheduler::new_task( mm::alloc_pages(), (void*)counting_test<'B', count_b> );


	// stay!
	while( 1 )
	{
	}

	taskX<'A'>();
	taskX<'B'>();
	taskX<'C'>();
}
