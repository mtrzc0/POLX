#include <dev/terminal/terminal.h>
#include <kernel/scheduler.h>
#include <kernel/klib.h>
#include <kernel/arch.h>
#include <kernel/task.h>
#include <kernel/vmm.h>
#include <kernel/vfs.h>

extern task_t *actual_task;
extern int errno;

vfs_node_ptr_t dev_stdin;

static struct {
	task_t *t;
	size_t size;
	uintptr_t buffer_userspace;
} wait4input;

long _stdin_read(vfs_node_ptr_t node, size_t offset, size_t size, char *buffer)
{
	(void)node;
	(void)offset;

	wait4input.t = actual_task;
	wait4input.size = size;
	wait4input.buffer_userspace = (uintptr_t)buffer;

	sched_sleep_task(actual_task->task_id);

	task_switch();

	return 0;
}

void dev_stdin_recall(char *input_line, size_t size)
{
	task_t *t = wait4input.t;
	size_t size_user = wait4input.size;
	uintptr_t dst = wait4input.buffer_userspace;

	/* Input is bigger than expected, copy only part of it */
	if (size_user < size) {
		if (vmm_copy_data_to(t->aspace, dst, input_line, size_user) < 0)
			regs_set_retval(t->regs, -1);
		else
			regs_set_retval(t->regs, size_user);
	} else {
		if (vmm_copy_data_to(t->aspace, dst, input_line, size) < 0)
			regs_set_retval(t->regs, -1);
		else
			regs_set_retval(t->regs, size);
	}

	/* Set errno if needed */
	if (errno > 0)
		regs_set_errno(t->regs, errno);

	wait4input.t = NULL;
	wait4input.size = 0;
	wait4input.buffer_userspace = 0;

	sched_wake_task(t->task_id);
}

vfs_node_ptr_t dev_stdin_init(void)
{
	vfs_node_ptr_t vnode;

	vnode = (vfs_node_ptr_t)kmalloc(sizeof(vfs_node_t));
	memset(vnode, 0, sizeof(vfs_node_t));
	
	strncpy(vnode->name, "stdin", 6);
	vnode->v_type = VFS_CHRDEV;
	vnode->op.read = &_stdin_read;

	dev_stdin = vnode;

	return vnode;
}
