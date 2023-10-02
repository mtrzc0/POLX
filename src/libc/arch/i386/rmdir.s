extern errno
global rmdir

; int rmdir(char *path);

section .text
rmdir:
	push ebp
	mov ebp, esp

	push ebx

	mov ebx, DWORD [ebp+0x8]

	mov eax, 0x10

	int 30h

	mov DWORD [errno], esi

	pop ebx

	leave
	ret
