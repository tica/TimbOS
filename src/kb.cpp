#include "system.h"
#include "kb.h"

#include "irq.h"

#include "console.h"

#include "debug.h"

#define SCROLL_LED 1
#define NUM_LED 2
#define CAPS_LED 4

/* KBDUS means US Keyboard Layout. This is a scancode table
*  used to layout a standard US keyboard. I have left some
*  comments in to give you an idea of what key is what, even
*  though I set it's array index to 0. You can change that to
*  whatever you want using a macro, if you wish! */
char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',		/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,					/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

char kbdus_upper[128] =
{
	0,  27, '!', '@', '#', '$', '%', '^', '&', '*',	/* 9 */
	'(', ')', '_', '+', '\b',	/* Backspace */
	'\t',			/* Tab */
	'Q', 'W', 'E', 'R',	/* 19 */
	'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n',		/* Enter key */
	0,			/* 29   - Control */
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',	/* 39 */
	'\'', '~',   0,		/* Left shift */
	'\\', 'Z', 'X', 'C', 'V', 'B', 'N',			/* 49 */
	'M', '<', '>', '?',   0,					/* Right shift */
	'*',
	0,	/* Alt */
	' ',	/* Space bar */
	0,	/* Caps lock */
	0,	/* 59 - F1 key ... > */
	0,   0,   0,   0,   0,   0,   0,   0,
	0,	/* < ... F10 */
	0,	/* 69 - Num lock*/
	0,	/* Scroll Lock */
	0,	/* Home key */
	0,	/* Up Arrow */
	0,	/* Page Up */
	'-',
	0,	/* Left Arrow */
	0,
	0,	/* Right Arrow */
	'+',
	0,	/* 79 - End key*/
	0,	/* Down Arrow */
	0,	/* Page Down */
	0,	/* Insert Key */
	0,	/* Delete Key */
	0,   0,   0,
	0,	/* F11 Key */
	0,	/* F12 Key */
	0,	/* All other keys are undefined */
};

//German Layout
char kbdger[128] =
{
	0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */	
	'9', '0', 'ß', '´', '\b',	/* Backspace */
	'\t',			/* Tab */
	'q', 'w', 'e', 'r',	/* 19 */
	't', 'z', 'u', 'i', 'o', 'p', 'ü', '+', '\n',		/* Enter key */
	0,			/* 29   - Control */
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'ö',	/* 39 */
	'\'', 'ä',   0,		/* Left shift */
	'\\', 'y', 'x', 'c', 'v', 'b', 'n',			/* 49 */
	'm', ',', '.', '-',   0,					/* Right shift */
	'*',
	0,	/* Alt */
	' ',	/* Space bar */
	0,	/* Caps lock */
	0,	/* 59 - F1 key ... > */
	0,   0,   0,   0,   0,   0,   0,   0,
	0,	/* < ... F10 */
	0,	/* 69 - Num lock*/
	0,	/* Scroll Lock */
	0,	/* Home key */
	0,	/* Up Arrow */
	0,	/* Page Up */
	'-',
	0,	/* Left Arrow */
	0,
	0,	/* Right Arrow */
	'+',
	0,	/* 79 - End key*/
	0,	/* Down Arrow */
	0,	/* Page Down */
	0,	/* Insert Key */
	0,	/* Delete Key */
	0,   0,   0,
	0,	/* F11 Key */
	0,	/* F12 Key */
	0	/* All other keys are undefined */
};

char kbdger_upper[128] =
{
	0,  27, '!', '"', '§', '$', '%', '&', '/', '(',	/* 9 */
	')', '=', '?', '`', '\b',	/* Backspace */
	'\t',			/* Tab */
	'Q', 'W', 'E', 'R',	/* 19 */
	'T', 'Z', 'U', 'I', 'O', 'P', 'Ü', '*', '\n',		/* Enter key */
	0,			/* 29   - Control */
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'Ö',	/* 39 */
	'\'', 'Ä',   0,		/* Left shift */
	'\\', 'Y', 'X', 'C', 'V', 'B', 'N',			/* 49 */
	'M', ';', ':', '_',   0,					/* Right shift */
	'*',
	0,	/* Alt */
	' ',	/* Space bar */
	0,	/* Caps lock */
	0,	/* 59 - F1 key ... > */
	0,   0,   0,   0,   0,   0,   0,   0,
	0,	/* < ... F10 */
	0,	/* 69 - Num lock*/
	0,	/* Scroll Lock */
	0,	/* Home key */
	0,	/* Up Arrow */
	0,	/* Page Up */
	'-',
	0,	/* Left Arrow */
	0,
	0,	/* Right Arrow */
	'+',
	0,	/* 79 - End key*/
	0,	/* Down Arrow */
	0,	/* Page Down */
	0,	/* Insert Key */
	0,	/* Delete Key */
	0,   0,   0,
	0,	/* F11 Key */
	0,	/* F12 Key */
	0,	/* All other keys are undefined */
};

static bool shift = false;

//Access the LEDs
void kbd_update_leds(uint8_t status)
{
	//uint8_t tmp;
	while((inportb(0x64)&2)!=0){} //loop until zero
	outportb(0x60,0xED);

	while((inportb(0x64)&2)!=0){} //loop until zero
	outportb(0x60,status);
}

//0 = numlock
//1 = capslock
//2 = scroll lock

static bool numlock;
static bool capslock;


void handle_lock_flags(unsigned char scancode)
{
	//num 69
	//caps 58
	//scroll not supported yet

	//numlock
	if(scancode == 69)
	{
		numlock = !numlock;
	}

	//caps
	if(scancode == 58)
	{
		capslock = !capslock;
	}
}

void handle_kbdleds()
{
	if(numlock && !capslock)
	{
		kbd_update_leds(NUM_LED);
	}
	else if(numlock && capslock)
	{
		kbd_update_leds(NUM_LED|CAPS_LED);
	}
	else if(!numlock && capslock)
	{
		kbd_update_leds(CAPS_LED);
	}
	else
	{
		kbd_update_leds(0);
	}
}

/* Handles the keyboard interrupt */
struct cpu_state* keyboard_handler(struct cpu_state *r)
{
    unsigned char scancode;

    /* Read from the keyboard's data buffer */
    scancode = inportb(0x60);

    /* If the top bit of the byte we read from the keyboard is
    *  set, that means that a key has just been released */
    if (scancode & 0x80)
    {
		scancode &= ~0x80;

        /* You can use this one to see if the user released the
        *  shift, alt, or control keys... */
		if(scancode == 0x2a || scancode == 0x36)
			shift = false;
    }
    else
    {
        /* Here, a key was just pressed. Please note that if you
        *  hold a key down, you will get repeated key press
        *  interrupts. */

		//2a = left shift
		//3a = capslock
		//36 = right shift
		if(scancode == 0x2a || scancode == 0x36)
			shift = true;

        /* Just to show you how this works, we simply translate
        *  the keyboard scancode into an ASCII value, and then
        *  display it to the screen. You can get creative and
        *  use some flags to see if a shift is pressed and use a
        *  different layout, or you can add another 128 entries
        *  to the above layout to correspond to 'shift' being
        *  held. If shift is held using the larger lookup table,
        *  you would add 128 to the scancode when you look for it */
        
		//The LEDs and the lockflags...
		handle_lock_flags(scancode);
		handle_kbdleds();

		char ch = shift ? kbdger_upper[scancode] : kbdger[scancode];

		debug_bochs_printf( "key event: scancode = %d, (0x%02X '%c')\n", scancode, ch, ch );

		if (kbdger[scancode])
		{
			console.printf("%c", ch);
		}
    }

	return r;
}

/* Installs the keyboard handler into IRQ1 */
void keyboard::init( void )
{
	outportb(0x60, 0xF0);
	outportb(0x60, 0x03);

	debug_bochs_printf("kb f0 00: 0x%02X\n", inportb(0x60));
	debug_bochs_printf("kb f0 00: 0x%02X\n", inportb(0x60));
	debug_bochs_printf("kb f0 00: 0x%02X\n", inportb(0x60));
	debug_bochs_printf("kb f0 00: 0x%02X\n", inportb(0x60));
	
	debug_bochs_printf( "keyboard_install..." );
    irq::install_handler(1, keyboard_handler);
	debug_bochs_printf( "done\n" );
}