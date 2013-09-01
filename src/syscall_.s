
[global int21_0]
int21_0:
	mov eax, [esp+4]
	int 0x21
	retn
