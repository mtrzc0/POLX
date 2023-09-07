extern errno
global mkdir

; int mkdir(char *path, int mode);

section .text
mkdir:
	push ebp
	mov ebp, esp

	push ebx
	push ecx

	mov ebx, DWORD [ebp+0x8]
	mov ecx, DWORD [ebp+0xc]

	mov eax, 0xe

	int 30h

	mov DWORD [errno], esi

	pop ecx
	pop ebx

	leave
	ret
