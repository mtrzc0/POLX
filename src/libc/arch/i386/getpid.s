extern errno
global getpid

; pid_t getpid(void);

section .text
getpid:
	push ebp
	mov ebp, esp

	mov eax, 0x3

	int 30h

	mov DWORD [errno], esi

	leave
	ret
