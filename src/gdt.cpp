
#include "system.h"
#include "gdt.h"

#include "debug.h"

#define GDT_ACCESS_PRESENT			0x80
#define GDT_ACCESS_RING_0			0x00
#define GDT_ACCESS_RING_3			0x60
#define GDT_ACCESS_CODE_SEG			0x1A
#define GDT_ACCESS_DATA_SEG			0x12
#define GDT_ACCESS_TSS				0x09
#define GDT_ACCESS_ACCESSED			0x01

#define GDT_FLAG_GRANULARITY_4K		0x8
#define GDT_FLAG_32BIT				0x4
#define GDT_FLAG_LONG_MODE			0x2
#define GDT_FLAG_USER				0x1

struct gdt_entry
{
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
} __ATTRIBUTE_PACKED__;

struct gdt_ptr
{
	unsigned short		limit;
	struct gdt_entry*	base;
} __ATTRIBUTE_PACKED__;

struct gdt_entry global_descriptor_table[6] = {};
struct gdt_ptr gp;

static uint32_t tss[32] = { 0, 0, 0x10 };

extern "C" void gdt_flush();

void gdt_set_entry(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char flags)
{
    /* Setup the descriptor base address */
    global_descriptor_table[num].base_low = (base & 0xFFFF);
    global_descriptor_table[num].base_middle = (base >> 16) & 0xFF;
    global_descriptor_table[num].base_high = (base >> 24) & 0xFF;

    /* Setup the descriptor limits */
    global_descriptor_table[num].limit_low = (limit & 0xFFFF);
    global_descriptor_table[num].granularity = ((limit >> 16) & 0x0F);

    /* Finally, set up the granularity and access flags */
    global_descriptor_table[num].granularity |= ((flags & 0xF) << 4);
    global_descriptor_table[num].access = access;
}

void gdt::init()
{
	debug_bochs_printf( "gdt_install... " );

    /* Setup the GDT pointer and limit */
    gp.limit = sizeof(global_descriptor_table) - 1;
    gp.base = global_descriptor_table;

    gdt_set_entry(0, 0, 0x00000, 0, 0);
	gdt_set_entry(1, 0, 0xFFFFF, GDT_ACCESS_PRESENT | GDT_ACCESS_CODE_SEG | GDT_ACCESS_RING_0, GDT_FLAG_GRANULARITY_4K | GDT_FLAG_32BIT);
    gdt_set_entry(2, 0, 0xFFFFF, GDT_ACCESS_PRESENT | GDT_ACCESS_DATA_SEG | GDT_ACCESS_RING_0, GDT_FLAG_GRANULARITY_4K | GDT_FLAG_32BIT);
	gdt_set_entry(3, 0, 0xFFFFF, GDT_ACCESS_PRESENT | GDT_ACCESS_CODE_SEG | GDT_ACCESS_RING_3, GDT_FLAG_GRANULARITY_4K | GDT_FLAG_32BIT);
    gdt_set_entry(4, 0, 0xFFFFF, GDT_ACCESS_PRESENT | GDT_ACCESS_DATA_SEG | GDT_ACCESS_RING_3, GDT_FLAG_GRANULARITY_4K | GDT_FLAG_32BIT);
	gdt_set_entry(5, (uintptr_t)tss, sizeof(tss) - 1, GDT_ACCESS_PRESENT | GDT_ACCESS_TSS | GDT_ACCESS_RING_3, 0);

    gdt_flush();

	asm volatile("ltr %%ax" : : "a" (5 << 3));

	debug_bochs_printf( "done\n" );
}

void gdt::update_tss_esp0( uintptr_t esp0 )
{
	tss[1] = esp0;
}