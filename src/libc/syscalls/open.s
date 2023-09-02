extern errno
global open

; int open(char *path, int flags);

section .text
open:
	push ebp
	mov ebp, esp

	push ebx
	push ecx

	mov ebx, DWORD [ebp+0x8]
	mov ecx, DWORD [ebp+0xc]

	mov eax, 0x9

	int 30h

	mov DWORD [errno], esi

	pop ecx
	pop ebx

	leave
	ret
	
