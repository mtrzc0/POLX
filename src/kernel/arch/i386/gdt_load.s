global gdt_load

section .text
gdt_load:
	push ebp
	mov ebp, esp
	push ebx ; Save previous value
	; Load argument from stack
	mov ebx, DWORD [ebp+0x8]   ; gdt pointer

	; Load GDTR register
	pushfd ; Push cpu flags
	cli    ; Disable interrupts
	lgdt [ebx]
	popfd  ; Pop cpu flags
	pop ebx ; Restore previous value
	jmp 0x08:.reload_segments ; Reload CS register

.reload_segments:
	push eax

	; Refresh data segment registers
	mov ax, 0x10
	mov ds, ax	; Data segment
	mov es, ax	; Extra segment
	mov fs, ax	; General purpose F segment
	mov gs, ax	; General purpose G segment

	pop eax
	leave
	ret
