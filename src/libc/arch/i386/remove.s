extern errno
global remove

; int remove(char *path);

section .text
remove:
	push ebp
	mov ebp, esp

	push ebx

	mov ebx, DWORD [ebp+0x8]

	mov eax, 0xe

	int 30h

	mov DWORD [errno], esi

	pop ebx

	leave
	ret
