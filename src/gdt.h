
#ifndef _GDT_H_INC_
#define _GDT_H_INC_

namespace gdt
{
	void init();
	void update_tss_esp0( uintptr_t esp0 );
}

#endif
