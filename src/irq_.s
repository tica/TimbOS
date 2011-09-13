
%macro irq_stub 1
	global irq%1
	irq%1:
		cli
		push byte 0
		push byte %1+32
		jmp irq_common_stub
%endmacro

irq_stub 0 ; 32: IRQ0
irq_stub 1 ; 33: IRQ1
irq_stub 2 ; 34: IRQ2
irq_stub 3 ; 35: IRQ3
irq_stub 4 ; 36: IRQ4
irq_stub 5 ; 37: IRQ5
irq_stub 6 ; 38: IRQ6
irq_stub 7 ; 39: IRQ7
irq_stub 8 ; 40: IRQ8
irq_stub 9 ; 41: IRQ9
irq_stub 10 ; 42: IRQ10
irq_stub 11 ; 43: IRQ11
irq_stub 12 ; 44: IRQ12
irq_stub 13 ; 45: IRQ13
irq_stub 14 ; 46: IRQ14
irq_stub 15 ; 47: IRQ15

extern irq_handler

irq_common_stub:
    pusha
    push ds
    push es
    push fs
    push gs

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp

    push eax
    mov eax, irq_handler
    call eax
    pop eax

    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret
