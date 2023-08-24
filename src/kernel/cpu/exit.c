#include <kernel/scheduler.h>
#include <kernel/syscalls.h>
#include <kernel/signal.h>
#include <kernel/klib.h>
#include <kernel/task.h>
#include <kernel/fd.h>

extern task_t *actual_task;

void do_exit(task_t *t, int status)
{
	task_t *parent;
	size_t fd;
	
	parent = t->parent;
	sched_remove(t);
	t->exit_code = status;

	if (t->task_id == 1) {
		task_switch_to(sched_get_next_task());
		//panic("Halting system\n");
	}

	/* Destroy used fds */
	fd = 0;
	while (t->fd_ctr > 0 && fd < MAX_FDS) {
		if (t->used_fd[fd] != NULL) {
			do_close(t, fd);
		}
		fd++;
	}

	/* Parent waits for child exit */
	if (parent->wait4->task_id == t->task_id) {
		t->state = ZOMBIE;
		sig_send(parent, SIGCHLD);
		sched_wake_task(parent->task_id);
		/* 
		   Don't destroy child tcb yet, 
		   it will be destroyed when 
		   SIGCHLD signal will be handled,
		   because parent want to know child
		   exit_code 
		*/
		return;
	}

	task_destroy(t);
	task_switch_to(sched_get_next_task());
}

void exit(int status)
{
	do_exit(actual_task, status);
}
