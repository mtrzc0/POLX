global context_switch
global syscall_return
extern tss_set_stack

section .text
context_switch:
	mov ebp, esp
	
	mov edx, DWORD [ebp+0x4] ; regs struct

	; Update TSS stack location 
	push DWORD [ebp + 0x8] ; kern_stack
	call tss_set_stack
	add esp, 0x4 ; Remove tss_set_stack args after call

	; Load task page directory
	;mov eax, DWORD [edx+0x4*10]
	;mov cr3, eax

ret_to_ring3:
	; Load task page directory
	mov eax, DWORD [edx+0x4*10]
	mov cr3, eax
	
	; Build int frame for iret
	mov eax, DWORD [edx+0x4*11] ; SS
	push eax
	mov eax, DWORD [edx+0x4*6]  ; ESP
	push eax
	mov eax, DWORD [edx+0x4*9]  ; EFLAGS
	push eax
	mov eax, DWORD [edx+0x4*12] ; CS
	push eax
	mov eax, DWORD [edx+0x4*8]  ; EIP
	push eax

	; Set segment registers
	mov eax, DWORD [edx+0x4*11]
	mov ds, ax
	mov gs, ax
	mov es, ax
	mov fs, ax

	; Load registers from TCB
	mov eax, DWORD [edx]
	mov ebx, DWORD [edx+0x4]
	mov ecx, DWORD [edx+0x4*2]
	mov esi, DWORD [edx+0x4*4]
	mov edi, DWORD [edx+0x4*5]
	mov ebp, DWORD [edx+0x4*7]
	push eax
	mov eax, DWORD [edx+0x4*3]
	mov edx, eax
	pop eax

	sti
	; Jump into ring 3 :)
	iret

syscall_return:
	mov ebp, esp
	mov edx, DWORD [ebp+0x4]
	jmp ret_to_ring3
