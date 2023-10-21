#include <kernel/scheduler.h>
#include <kernel/syscalls.h>
#include <kernel/signal.h>
#include <kernel/task.h>
#include <kernel/klib.h>
#include <errno.h>

extern int errno;
extern task_t *actual_task;

int do_waitpid(task_t *t, pid_t pid)
{
	task_t *tcb;

	tcb = sched_get_tcb(pid);

	if (tcb == NULL) {
		errno = ESRCH;
		return -1;
	}

	t->wait4 = tcb;
	SIG_MARK(t->mask, SIGCHLD);

	sched_sleep_task(t->task_id);
	
	//kprintf("[DEBUG] Task %d is waiting for child\n", t->task_id);
	
	task_switch();

	return 0;
}

int waitpid(pid_t pid)
{
	return do_waitpid(actual_task, pid);
}
