extern errno
global fork

; pid_t fork(void); 

section .text
fork:
	push ebp
	mov ebp, esp

	mov eax, 0x1

	int 30h 

	; errno
	mov DWORD [errno], ebx

	leave
	ret
