
[global interlocked_exchange]
	; uint32_t interlocked_exchange( uint32_t* p, uint32_t x );
interlocked_exchange:
	mov ecx, [esp+4]	; ecx = p
	mov eax, [esp+8]	; eax = x
	xchg eax, [ecx]
    ret


[global interlocked_compare_exchange]
	; uint32_t interlocked_compare_exchange( uint32_t* p, uint32_t comp, uint32_t exch );
	; [esp+4] = p
	; [esp+8] = comp
	; [esp+12] = exch
interlocked_compare_exchange:
	mov eax, [esp+8]			; eax = comp
	mov ecx, [esp+12]			; ecx = exch
	mov edx, [esp+4]			; edx = p
	lock cmpxchg [edx], ecx
	ret

[global interlocked_compare_exchange_64]
	; uint64_t interlocked_compare_exchange_64( uint64_t* p, uint64_t comp, uint64_t exch );
	; [esp+4] = p = [ebp+8 after push ebp, mov ebp esp]
	; [esp+8] = comp = [ebp+12 after push ebp, mov ebp esp]
	; [esp+16] = exch = [ebp+20 after push ebp, mov ebp esp]
interlocked_compare_exchange_64:

	push ebp
	mov ebp, esp

	push ebx
	push edi	

	mov edx, [ebp+16]	;
	mov eax, [ebp+12]	;	edx:eax = comp

	mov ecx, [ebp+24]	;
	mov ebx, [ebp+20]	;	ecx:ebx = exch

	mov edi, [ebp+8]	;	edi = p

	lock cmpxchg8b	[edi]

	pop edi
	pop ebx

	pop ebp

	ret

[global interlocked_increment]
	;	uint32_t	interlocked_increment( uint32_t* p );
	;	[esp+4] = p
interlocked_increment:
	mov ecx, [esp+4]		; ecx = p	
	xor eax, eax			; eax = 0
	inc eax					; eax = 1
	lock xadd [ecx], eax	; *p += eax | eax = *p
	inc eax					; eax += 1
	ret


[global interlocked_decrement]
	;	uint32_t	interlocked_decrement( uint32_t* p );
	;	[esp+4] = p
interlocked_decrement:
	mov ecx, [esp+4]		; ecx = p	
	xor eax, eax			; eax = 0
	dec eax					; eax = -1
	lock xadd [ecx], eax	; *p += eax | eax = *p
	dec eax					; eax -= 1
	ret