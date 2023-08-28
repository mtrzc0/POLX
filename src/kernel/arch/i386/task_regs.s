global set_stack_pointer
global get_stack_pointer

section .text
set_stack_pointer:
	push ebp
	mov ebp, esp
	mov esp, DWORD [ebp+0x8]
	push DWORD [ebp+0x4] ; Copy return address from old stack
	mov ebp, esp

	ret

get_stack_pointer:
	push ebp
	mov ebp, esp

	mov eax, esp
	
	leave
	ret
