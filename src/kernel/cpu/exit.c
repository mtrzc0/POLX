#include <kernel/scheduler.h>
#include <kernel/syscalls.h>
#include <kernel/signal.h>
#include <kernel/klib.h>
#include <kernel/arch.h>
#include <kernel/task.h>
#include <kernel/fd.h>

extern task_t *actual_task;
extern task_t *kernel_task;

static task_t *orig;

void do_exit(task_t *t, int status)
{
	task_t *parent;
	size_t fd;
	
	parent = t->parent;
	sched_remove(t);
	t->exit_code = status;

	//kprintf("[DEBUG] Task %d exited with %d code\n", actual_task->task_id, status);
	
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
	if (parent->wait4 != NULL) {
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
			set_stack_pointer(regs_get_stack_pointer(kernel_task->regs));
			task_switch_to(sched_get_next_task());
			return;
		}
	}

	orig = t;

	/*
	   Kernel is currently running on kernel stack used by syscalls,
	   from task->kernel_stack, so before we will destroy this task
	   and its kernel stack we have to switch to new stack
	*/
	set_stack_pointer(regs_get_stack_pointer(kernel_task->regs));
	task_destroy(orig);
	task_switch_to(sched_get_next_task());
}

void exit(int status)
{
	do_exit(actual_task, status);
}
