extern errno
global close

; int close(int fd);

section .text
close:
	push ebp
	mov ebp, esp

	push ebx

	mov ebx, DWORD [ebp+0x8]
	
	mov eax, 0xa

	int 30h

	mov DWORD [errno], esi
	
	pop ebx

	leave
	ret
