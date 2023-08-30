extern errno
global execve

; int execve(const char *path, char *const argv[]);

section .text
execve:
	push ebp
	mov ebp, esp

	push ebx
	push ecx

	mov ebx, DWORD [ebp+0x8]
	mov ecx, DWORD [ebp+0xc]

	mov eax, 0x2

	int 30h

	mov DWORD [errno], esi

	pop ecx
	pop ebx

	leave
	ret
