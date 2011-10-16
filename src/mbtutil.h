
#ifndef _MBTUTIL_H_INC_
#define _MBTUTIL_H_INC_

#include "multiboot.h"
#include "elf.h"

namespace multiboot
{
	template<typename TCallback>
	void iterate_mmap( const multiboot_info* mbt, TCallback cb /*void cb(const multiboot_mmap_entry*)*/ )
	{
		if( mbt->flags & MULTIBOOT_INFO_MEM_MAP )
		{
			const multiboot_mmap_entry* mmap = reinterpret_cast<const multiboot_mmap_entry*>( mbt->mmap_addr );
			const multiboot_mmap_entry* mmap_end = reinterpret_cast<const multiboot_mmap_entry*>( mbt->mmap_addr + mbt->mmap_length );
		
			while( mmap < mmap_end )
			{
				cb( mmap );
				mmap = (multiboot_mmap_entry*) ( uintptr_t(mmap) + sizeof(mmap->size) + mmap->size );
			}
		}
	}

	template<typename TCallback>
	void iterate_elf_section_headers( const multiboot_info* mbt, Elf32_Word flag_mask, TCallback cb )
	{
		if( mbt->flags & MULTIBOOT_INFO_ELF_SHDR )
		{
			const multiboot_elf_section_header_table& esht = mbt->u.elf_sec;
			const Elf32_Shdr* esh = reinterpret_cast<const Elf32_Shdr*>( esht.addr );

			for( size_t i = 0; i < esht.num; ++i )
			{
				if( esh->sh_flags & flag_mask )
				{
					cb( esh );
				}
				esh++;
			}
		}
	}
}

#endif // _MBTUTIL_H_INC_
