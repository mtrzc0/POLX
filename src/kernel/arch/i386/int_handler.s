extern general_handler
global isr_vectors

%macro with_error_code 1
isr_%+%1:
	push %1	; Push interrupt number
	jmp int_wrapper
%endmacro

%macro without_error_code 1
isr_%+%1:
	push 0	; Simulate error code for int_frame_t 
	push %1
	jmp int_wrapper
%endmacro

%macro ax_to_seg_regs 0
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
%endmacro

section .text
int_wrapper:
	cli

	mov [frame_ptr], esp
	pushad	; Save gp registers
	
	mov ax, ds
	push eax ; Save seg registes value

	; Ensure that we operate in kernel segments
	mov ax, 0x10
	ax_to_seg_regs

	; Push arguments and call handler

	mov eax, esp
	push eax ; &int_regs_t

	mov eax, [frame_ptr]
	push eax ; &int_frame_t
	call general_handler

	; Remove arguments from stack
	add esp, 0x8

	; Restore state of interrupted program
	pop eax
	ax_to_seg_regs
	popad

	; Remove exception number and error code
	add esp, 0x8

	; Enable interrupts and return
	sti
	iret

; Define exceptions labels
without_error_code	0
without_error_code	1
without_error_code	2
without_error_code	3
without_error_code	4
without_error_code	5
without_error_code	6
without_error_code	7
with_error_code		8
without_error_code	9
with_error_code		10
with_error_code		11
with_error_code		12
with_error_code		13
with_error_code		14
without_error_code	15
without_error_code	16
with_error_code		17
without_error_code	18
without_error_code	19
without_error_code	20
without_error_code	21
without_error_code	22
without_error_code	23
without_error_code	24
without_error_code	25
without_error_code	26
without_error_code	27
without_error_code	28
without_error_code	29
with_error_code		30
without_error_code	31
; IRQS
without_error_code	32
without_error_code	33
without_error_code	34
without_error_code	35
without_error_code	36
without_error_code	37
without_error_code	38
without_error_code	39
without_error_code	40
without_error_code	41
without_error_code	42
without_error_code	43
without_error_code	44
without_error_code	45
without_error_code	46
without_error_code	47
; SYSCALLS TRIGGER
without_error_code	48

section .bss
frame_ptr:
	resb 4

section .data
; Create isr_vectors array
isr_vectors:
%assign i 0
%rep 49
	dd isr_%+i
%assign i i+1
%endrep
