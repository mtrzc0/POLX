#include <stdint.h>
#include <i386/isr.h>
#include <i386/irq.h>
#include <i386/idt.h>
#include <i386/paging.h>
#include <i386/context_switch.h>
#include <kernel/klib.h>
#include <kernel/task.h>
#include <kernel/syscalls.h>
#include <dev/timer.h>

int errno;
extern task_t *actual_task;
extern uint32_t kern_pd_paddr;

char *exception_name[] = {
	"Divide Error",
	"Debug Exception",
	"NMI Interrupt",
	"Breakpoint",
	"Overflow",
	"Bounds Check",
	"Invalid Opcode",
	"Coprocessor Not Available",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Invalid TSS",
	"Segment Not Present",
	"Stack Exception",
	"General Protection Exception(Triple Fault)",
	"Page Fault",
	"Intel reserved",
	"FPU Floating-Point Error",
	"Alignment Check",
	"Machine Check",
	"SIMD Floating-Point Exception",
	"Virtualization Exception",
	"Control Protection Exception",
	"Intel reserved",
	"Intel reserved",
	"Intel reserved",
	"Intel reserved",
	"Intel reserved",
	"Intel reserved",
	"Intel reserved",
	"Intel reserved",
	"Intel reserved",
	"Intel reserved",
};

static void _exception_handler(int_frame_t *int_frame)
{
	switch(int_frame->int_number) {
	case 14:
		pg_page_fault_handler(int_frame->err_code);
		break;
	default:
		panic(exception_name[int_frame->int_number]);	
	}

	/* User mode task interrupted */
	if (int_frame->eip < VM_KERN_START)
		syscall_return(&actual_task->regs);
}

static void _irq_handler(int_frame_t *int_frame)
{
	
	switch(int_frame->int_number) {
	case IRQ0:
		if (timer_isr()) {
			outb(PIC1_COMM, PIC_EOI);
			task_switch();
		}
		break;
	default:
		kprintf("IRQ nr: %d not implemented\n", int_frame->int_number);
	}

	outb(PIC1_COMM, PIC_EOI);
	if (int_frame->int_number >= 40)
		outb(PIC2_COMM, PIC_EOI);
	
	
	/* User mode task interrupted */
	if (int_frame->eip < VM_KERN_START)
		syscall_return(&actual_task->regs);
}

static void _syscall_handler(void)
{
	task_regs_t *r;
	pid_t p;
	int fd, ret_int;
	long ret_long;

	r = &actual_task->regs;

	/* Syscall number */
	switch(r->eax) {
	case 0: // exit
		exit(r->ebx);
		break;
	case 1: // fork
		p = fork();
		regs_set_retval(*r, p);
		break;
	case 2: // execve
		ret_int = execve((char *)r->ebx, (char **)r->ecx);
		if (ret_int < 0)
			regs_set_retval(*r, ret_int);
		break;
	case 3: // getpid
		p = getpid();
		regs_set_retval(*r, p);
		break;
	case 4: // getppid
		p = getppid();
		regs_set_retval(*r, p);
		break;
	case 5: // waitpid
		break;
	case 6: // mmap
		break;
	case 7: // brk
		break;
	case 8: // sbrk
		break;
	case 9: // open
		fd = open((char *)r->ebx, r->ecx);
		regs_set_retval(*r, fd);
		break;
	case 10: // close
		fd = close(r->ebx);
		regs_set_retval(*r, fd);
		break;
	case 11: // read
		break;
	case 12: // write
		ret_long = write(r->ebx, (void *)r->ecx, r->edx);
		regs_set_retval(*r, ret_long);
		break;
	case 13:
		// remove
		break;
	case 14:
		// mkdir
		break;
	case 15:
		// rmdir
		break;
	case 16:
		// readdir
		break;
	case 17:
		// kill
		break;
	
	}

	if (errno < 0)
		regs_set_errno(*r, errno);
	
	syscall_return(r);
}

void general_handler(int_frame_t *int_frame, int_regs_t *task_regs)
{
	errno = 0;

	/* Scheduler is initialized and it is not an empty task */
	if (actual_task != NULL && actual_task->task_id > 0) {
		/* Interrupt occured in user mode */
		if (int_frame->eip < VM_KERN_START) {
			actual_task->regs.eax = task_regs->eax;
			actual_task->regs.ebx = task_regs->ebx;
			actual_task->regs.ecx = task_regs->ecx;
			actual_task->regs.edx = task_regs->edx;
			actual_task->regs.esi = task_regs->esi;
			actual_task->regs.edi = task_regs->edi;
			actual_task->regs.ebp = task_regs->ebp;
			actual_task->regs.esp = int_frame->esp;
			actual_task->regs.eip = int_frame->eip;
			actual_task->regs.eflags = int_frame->eflags;
			//actual_task->regs.ds = task_regs->ds;
			//actual_task->regs.cs = int_frame->cs;
	
			/* Set kernel CR3 */
			pg_switch_pd((uint32_t)kern_pd_paddr);
		}
	}

	if (int_frame->int_number < IDT_CPU_EXCEPTIONS)
		_exception_handler(int_frame);
	else if (int_frame->int_number < IDT_CPU_EXCEPTIONS + IRQ_QUANTITY)
		_irq_handler(int_frame);
	else
		_syscall_handler();
}

