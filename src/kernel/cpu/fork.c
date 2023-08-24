#include <kernel/scheduler.h>
#include <kernel/syscalls.h>
#include <kernel/arch.h>
#include <kernel/task.h>
#include <kernel/vmm.h>
#include <kernel/fd.h>

extern task_t *actual_task;

pid_t do_fork(task_t *t)
{
	task_t *new_task;
	vmm_aspace_t *new_as;

	/* Address space */
	new_as = (vmm_aspace_t *)vmm_aspace_create(t->aspace);

	/* Process context */
	new_task = (task_t *)task_new(t->priority, new_as);
	regs_fork(t->regs, new_task->regs);

	/* Signals */
	new_task->mask = t->mask;

	/* IO subsystem */
	for (size_t i=0, fd_ctr=0; i < MAX_FDS && fd_ctr < t->fd_ctr; i++) {
		/* Copy parent fd */
		if (t->used_fd[i] != NULL) {
			new_task->used_fd[i] = t->used_fd[i];
			fd_ctr++;
		}
	}
	new_task->fd_ctr = t->fd_ctr;

	/* Add new task to scheduler queue */
	sched_add_task(new_task);

	/* Children return value */
	regs_set_retval(new_task->regs, 0);
	
	/* Parent return value */
	return new_task->task_id;
}

pid_t fork(void)
{
	return do_fork(actual_task);
}
