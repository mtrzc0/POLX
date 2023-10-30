#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <kernel/vmm.h>
#include <kernel/klib.h>
#include <kernel/task.h>
#include <kernel/arch.h>
#include <kernel/devfs.h>
#include <kernel/signal.h>
#include <kernel/initrd.h>
#include <kernel/syscalls.h>
#include <kernel/scheduler.h>

extern int errno;
extern uint32_t kern_pd_paddr;
extern task_t *kernel_task;

uint32_t kernel_end;

void kernel_main(multiboot_info_t *mb)
{
	multiboot_module_t *mods;
	vmm_aspace_t *init_as;
	task_t *init;
	fd_t *stdout_fd, *stdin_fd;
	char *argv[2];

	/* Initialize GDT, IDT, PMM and so on */
	arch_init(mb);

	/* Initialize CPU scheduler */
	sched_init();

	/* Mount initial ram disk */
	mods = (multiboot_module_t *) (mb->mods_addr + VM_KERN_START);
	initrd_load((mods[0].mod_start + VM_KERN_START));

	/* Initialize devfs */
	devfs_init();

	/* Prepare init program context */
	argv[0] = "/bin/init";
	argv[1] = NULL;
	init_as = (vmm_aspace_t *)kmalloc(sizeof(vmm_aspace_t));
	init_as->pd = kern_pd_paddr;
	init = task_new(OS, init_as);
	init->mask = SIG_DEFAULT_MASK;

	init->regs.cs = 0x18 | 3;
	init->regs.ds = 0x20 | 3;

	if (do_execve(init, "/bin/init", argv) != 0)
		panic("[ERROR] Failed to load init program");
	
	/* Initialize stdin, stdout file descriptors */
	extern vfs_node_ptr_t dev_stdin;
	extern vfs_node_ptr_t dev_stdout;
	
	stdin_fd = kmalloc(sizeof(fd_t));
	stdin_fd->position = 0;
	stdin_fd->mode = O_RDONLY;
	stdin_fd->vfs_node = dev_stdin;
	fd_add(stdin_fd);
	
	stdout_fd = kmalloc(sizeof(fd_t));
	stdout_fd->position = 0;
	stdout_fd->mode = O_WRONLY;
	stdout_fd->vfs_node = dev_stdout;
	fd_add(stdout_fd);

	init->used_fd[0] = stdin_fd;
	init->used_fd[1] = stdout_fd;
	init->fd_ctr += 2;

	kernel_task->used_fd[0] = stdin_fd;
	kernel_task->used_fd[1] = stdout_fd;
	kernel_task->fd_ctr += 2;

	/* Announcement */
	kprintf("*******************************************************************************\n");
	kprintf("Welcome to TrytonOS\n");
	kprintf("This is a hobby project created by determined self-taught individual\n");
	kprintf("*******************************************************************************\n");

	/* Execute init program */
	sched_add_task(init);
	irq_unmask_int(IRQ0);
	task_switch();
}
