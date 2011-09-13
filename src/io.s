
[global outportb]
outportb:
	mov eax, [esp+8]
	mov edx, [esp+4]
	out dx, al
	ret

[global outportw]
outportw:
	mov eax, [esp+8]
	mov edx, [esp+4]
	out dx, ax
	ret

[global inportb]
inportb:
	mov edx, [esp+4]
	in al, dx
	ret