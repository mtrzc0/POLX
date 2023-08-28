#ifndef __task_regs_dot_H
#define __task_regs_dot_H
#include <stdint.h>


/* Hardcoded in context_switch assembly, do not change order! */
struct task_regs {
	uint32_t eax, ebx, ecx, edx, esi, edi;
	uint32_t esp, ebp, eip, eflags, cr3;
	uint32_t ds, cs;
} __attribute__((packed));
typedef struct task_regs task_regs_t;

/* Syscall init */
/* EAX - syscall number */
/* EBX - 1 argument */
/* ECX - 2 argument */
/* EDX - 3 argument */
/* ESI - 4 argument */
/* EDI - 5 argument */
/* EBP - 6 argument */

/* Syscall end */
/* EAX - Return value */
/* EBX - errno */

/* execve arguments for _init */
/* EAX - argc */
/* EBX - argv address */

void set_stack_pointer(uint32_t esp);
uint32_t get_stack_pointer(void);

#define regs_set_pd(regs_ptr, pd) \
	(regs_ptr).cr3 = (pd)

#define regs_set_retval(regs_ptr, val) \
	(regs_ptr).eax = (val)

#define regs_set_errno(regs_ptr, err) \
	(regs_ptr).ebx = (err)

#define regs_set_instruction_pointer(regs_ptr, ip) \
	(regs_ptr).eip = (ip)

#define regs_set_stack_pointer(regs_ptr, sp) \
	(regs_ptr).esp = (sp)

#define regs_get_stack_pointer(regs_ptr) \
	({uint32_t esp; esp = (regs_ptr).esp; esp;})

#define regs_set_execve_args(regs_ptr, argc, argv) \
	(regs_ptr).eax = (argc); \
	(regs_ptr).ebx = (argv)

#define regs_fork(src_ptr, dst_ptr) \
	(dst_ptr).eax = (src_ptr).eax; \
	(dst_ptr).ebx = (src_ptr).ebx; \
	(dst_ptr).ecx = (src_ptr).ecx; \
	(dst_ptr).edx = (src_ptr).edx; \
	(dst_ptr).esi = (src_ptr).esi; \
	(dst_ptr).edi = (src_ptr).edi; \
	(dst_ptr).esp = (src_ptr).esp; \
	(dst_ptr).ebp = (src_ptr).ebp; \
	(dst_ptr).eip = (src_ptr).eip; \
	(dst_ptr).ds = (src_ptr).ds; \
	(dst_ptr).cs = (src_ptr).cs; \
	(dst_ptr).eflags = (src_ptr).eflags

#endif
