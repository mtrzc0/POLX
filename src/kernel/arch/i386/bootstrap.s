global start
global kern_pd
extern kernel_main
extern kernel_paddr_end 

%define virt2phys -0xc0000000
%define phys2virt 0xc0000000

section .multiboot
	MB_ALIGN	equ 1 << 0
	MB_MEMINFO	equ 1 << 1
	MB_FLAGS	equ MB_ALIGN | MB_MEMINFO
	MB_MAGIC	equ 0x1BADB002
	MB_CHECKSUM	equ -(MB_MAGIC + MB_FLAGS)

	align 4
	dd MB_MAGIC
	dd MB_FLAGS
	dd MB_CHECKSUM

section .bootstrap
start:
	mov edx, ebx	; Save multiboot_info struct address
	add edx, phys2virt
	mov eax, kern_stack
	add eax, virt2phys
	mov esp, eax	; temporary boot stack
	push edx

; Page tables fill
; kern_end = (kernel_paddr_end % 0x1000 == 0) ? kernel_paddr_end / 4096 : kernel_paddr_end / 4096 + 1
; for (uint32_t i=0, addr=0; i < kern_end; i++, addr+=4096) {
;	identity_pt[i] = addr | 3; 
;	kern_pt[i] = addr | 3;
; }
	mov edx, 0
	mov eax, kernel_paddr_end
	mov ecx, 0x1000
	div ecx

	cmp edx, 0
	jne .reminder
	mov edx, eax
	jmp .loop
	
	.reminder:
		inc eax
		mov edx, eax

	.loop:
	mov eax, 0x0
	mov ebx, 0x0
	.fill_page_tables:
		mov ecx, ebx
		or  ecx, 3	; WARNING: this make page writable even .text and .rodata segments!
		mov [kern_pt+virt2phys+eax*4], ecx
		add ebx, 4096
		inc eax
		cmp eax, edx
		je .end_fill
		jmp .fill_page_tables
	.end_fill:

; Prepare pd entries
	mov eax, kern_pt
	add eax, virt2phys
	and eax, 0xFFFFF000
	or  eax, 3

; Move entries to page dir
	mov ecx, kern_pd
	add ecx, virt2phys
	mov [ecx], eax
	mov [ecx+768*4], eax

; Make recursive page directory
	mov eax, ecx
	or eax, 3
	mov [ecx+1023*4], eax

; Load page directory
	mov cr3, ecx

; Enable paging
	mov eax, cr0
	or  eax, 0x80000001
	mov cr0, eax

	pop edx		; Remove mb_info from boot stack

; Jump to higher half
	lea eax, [.higher_half_start]
	jmp eax

section .text
.higher_half_start:
; Unmap identity page table
	mov DWORD [kern_pd], 0x0

; Flush TLB
	mov eax, cr3
	mov cr3, eax

; Now we are in 0xC0100000 :D

; Kernel initialization
	mov esp, kern_stack ; Kernel stack
	push edx	; Push mb_info
	call kernel_main

	cli
	hlt
	jmp $

section .bss
align 4
kern_pd:
	resb 0x1000

; (0x0, 4MiB) to (0xC0000000, +4MiB)
kern_pt:
	resb 0x1000

kern_stack_end:
	resb 0x4000 ; 16 KiB
kern_stack:
