extern errno
global brk

; int brk(void *addr);

section .text
brk:
	push ebp
	mov ebp, esp

	push ebx

	mov ebx, DWORD [ebp+0x8]

	mov eax, 0x8

	int 30h

	mov DWORD [errno], esi

	pop ebx

	leave
	ret
