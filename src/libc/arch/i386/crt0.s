extern main
global _start
global errno

section .text

_start:
	push ebp
	mov ebp, esp

	; argv ptr
	push ebx
	; argc 
	push eax

	call main

	jmp _fini

_fini:
	; return value 
	mov ebx, eax
	; Syscall no.0 exit 
	xor eax, eax

	int 30h

section .bss
align 4
errno:
	resb 0x4
