extern errno
global readdir

; int readdir(int fd, struct dirent *dent);

section .text
readdir:
	push ebp
	mov ebp, esp

	push ebx
	push ecx

	mov ebx, DWORD [ebp+0x8]
	mov ecx, DWORD [ebp+0xc]

	mov eax, 0x11

	int 30h

	mov DWORD [errno], esi

	pop ecx
	pop ebx

	leave
	ret
