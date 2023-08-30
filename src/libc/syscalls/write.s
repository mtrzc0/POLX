extern errno
global write

; long write(int fd, const void *buff, size_t size);

section .text
write:
	push ebp
	mov ebp, esp

	push ebx
	push ecx
	push edx

	mov ebx, DWORD [ebp + 0x8]
	mov ecx, DWORD [ebp + 0xc]
	mov edx, DWORD [ebp + 0x10]

	mov eax, 0xc

	int 30h 

	; errno
	mov DWORD [errno], esi

	pop edx
	pop ecx
	pop ebx

	leave
	ret
