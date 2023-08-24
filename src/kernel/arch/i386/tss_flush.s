global tss_flush

section .text
tss_flush:
	push ebp
	mov ebp, esp
	push eax
	
	mov ax, 0x28 ; Offset of tss descriptor in GDT
	ltr ax

	pop eax
	leave
	ret
