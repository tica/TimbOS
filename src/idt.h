
#ifndef _IDT_H_INC_
#define _IDT_H_INC_

namespace idt
{
	void init();
	void set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
}

#endif // _IDT_H_INC_
