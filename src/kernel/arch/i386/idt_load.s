global idt_load

section .text
idt_load:
	push ebp
	mov ebp, esp
	push eax

	; Load IDT pointer
	mov eax, DWORD [ebp+0x8]
	
	; Load IDTR register
	pushfd
	cli
	lidt [eax]
	popfd

	pop eax
	leave
	ret
