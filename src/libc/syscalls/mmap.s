extern errno
global mmap

; void *mmap(void *addr, size_t size, int prot, int fd, size_t offset);

section .text
mmap:
	push ebp
	mov ebp, esp

	push ebx
	push ecx
	push edx
	push esi
	push edi

	mov ebx, DWORD [ebp+0x8]
	mov ecx, DWORD [ebp+0xc]
	mov edx, DWORD [ebp+0x10]
	mov esi, DWORD [ebp+0x14]
	mov edi, DWORD [ebp+0x18]

	mov eax, 0x6

	int 30h

	mov DWORD [errno], esi

	pop edi
	pop esi
	pop edx
	pop ecx
	pop ebx

	leave
	ret

