extern errno
global lseek

; long lseek(int fd, long offset, int whence);

section .text
lseek:
	push ebp
	mov ebp, esp

	push ebx
	push ecx
	push edx

	mov ebx, DWORD [ebp+0x8]
	mov ecx, DWORD [ebp+0xc]
	mov edx, DWORD [ebp+0x10]

	mov eax, 0x12

	int 30h

	mov DWORD [errno], esi

	pop edx
	pop ecx
	pop ebx

	leave
	ret
