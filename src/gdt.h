
#ifndef _GDT_H_INC_
#define _GDT_H_INC_

namespace gdt
{
	void init();
	void update_tss_esp0( void* esp0 );
}

#endif
