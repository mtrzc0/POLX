global exit

; void exit(int status);

section .text
exit:
	push ebp
	mov ebp, esp

	mov ebx, DWORD [ebp+0x8]

	mov eax, 0x0

	int 30h

	leave
	ret
