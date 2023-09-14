extern errno
global sbrk

; void *sbrk(intptr_t inc);

section .text
sbrk:
	push ebp
	mov ebp, esp

	push ebx

	mov ebx, DWORD [ebp+0x8]

	mov eax, 0x9

	int 30h

	mov DWORD [errno], esi

	pop ebx

	leave
	ret
