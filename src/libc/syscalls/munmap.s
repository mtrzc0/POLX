extern errno
global munmap

; int munmap(void *addr);

section .text
munmap:
	push ebp
	mov ebp, esp

	push ebx

	mov ebx, DWORD [ebp+0x8]

	mov eax, 0x7

	int 30h

	mov DWORD [errno], esi

	pop ebx

	leave
	ret
