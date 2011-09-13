
%macro isr_stub 1
	global isr%1
	isr%1:
		cli
		push byte 0
		push byte %1
		jmp isr_common_stub
%endmacro

%macro isr_stub_e 1
	global isr%1
	isr%1:
		cli
		push byte %1
		jmp isr_common_stub
%endmacro

isr_stub	0	;  0: Divide By Zero Exception
isr_stub	1	;  1: Debug Exception
isr_stub	2	;  2: Non Maskable Interrupt Exception
isr_stub	3	;  3: Int 3 Exception
isr_stub	4	;  4: INTO Exception
isr_stub	5	;  5: Out of Bounds Exception
isr_stub	6	;  6: Invalid Opcode Exception
isr_stub	7	;  7: Coprocessor Not Available Exception
isr_stub_e	8	;  8: Double Fault Exception (With Error Code!)
isr_stub	9	;  9: Coprocessor Segment Overrun Exception
isr_stub_e	10	; 10: Bad TSS Exception (With Error Code!)
isr_stub_e	11	; 11: Segment Not Present Exception (With Error Code!)
isr_stub_e	12	; 12: Stack Fault Exception (With Error Code!)
isr_stub_e	13	; 13: General Protection Fault Exception (With Error Code!)
isr_stub_e	14	; 14: Page Fault Exception (With Error Code!)
isr_stub	15	; 15: Reserved Exception
isr_stub	16	; 16: Floating Point Exception
isr_stub	17	; 17: Alignment Check Exception
isr_stub	18	; 18: Machine Check Exception
isr_stub	19	; 19: Reserved
isr_stub	20	; 20: Reserved
isr_stub	21	; 21: Reserved
isr_stub	22	; 22: Reserved
isr_stub	23	; 23: Reserved
isr_stub	24	; 24: Reserved
isr_stub	25	; 25: Reserved
isr_stub	26	; 26: Reserved
isr_stub	27	; 27: Reserved
isr_stub	28	; 28: Reserved
isr_stub	29	; 29: Reserved
isr_stub	30	; 30: Reserved
isr_stub	31	; 31: Reserved



; We call a C function in here. We need to let the assembler know
; that 'fault_handler' exists in another file
extern fault_handler

; This is our common ISR stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
isr_common_stub:
	push eax
	push edx
	mov al, 0x41
	mov dx, 0xe9
	out dx, al
	pop eax
	pop edx
	
    pushad
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
    mov eax, fault_handler
    call eax
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popad
    add esp, 8
    iret
