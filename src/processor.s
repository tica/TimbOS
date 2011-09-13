
[global interrupts_enable]
interrupts_enable:
	sti
	retn
	
[global interrupts_disable]
interrupts_disable:
	cli
	retn	

[global read_cr0]
read_cr0:
	mov eax, cr0
	retn
	
[global read_cr2]
read_cr2:
	mov eax, cr2
	retn
	
[global read_cr3]
read_cr3:
	mov eax, cr3
	retn
	
[global read_flags]
read_flags:
	pushf
	pop eax
	retn
