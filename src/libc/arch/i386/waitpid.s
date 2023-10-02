extern errno
global waitpid

; int waitpid(pid_t pid);

section .text
waitpid:
	push ebp
	mov ebp, esp

	push ebx

	mov ebx, DWORD [ebp+0x8]

	mov eax, 0x5

	int 30h

	mov DWORD [errno], esi

	pop ebx

	leave
	ret
