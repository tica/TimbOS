
[global interlocked_exchange]
	; int interlocked_exchange( int* p, int x );
interlocked_exchange:
	mov ecx, [esp+4]	; ecx = p
	mov eax, [esp+8]	; eax = x
	xchg eax, [ecx]
    ret


[global interlocked_compare_exchange]
	; int interlocked_compare_exchange( int* p, int comp, int exch );
	; [esp+4] = p
	; [esp+8] = comp
	; [esp+12] = exch
interlocked_compare_exchange:
	mov eax, [esp+8]			; eax = comp
	mov ecx, [esp+12]			; ecx = exch
	mov edx, [esp+4]			; edx = p
	lock cmpxchg [edx], ecx
	ret