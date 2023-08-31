
extern errno
global getppid

; pid_t getppid(void);

section .text
getppid:
	push ebp
	mov ebp, esp

	mov eax, 0x4

	int 30h

	mov DWORD [errno], esi

	leave
	ret
