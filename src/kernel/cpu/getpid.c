#include <kernel/syscalls.h>
#include <kernel/task.h>

extern task_t *actual_task;

pid_t getpid(void)
{
	return (pid_t)actual_task->task_id;
}

pid_t getppid(void)
{
	pid_t p;
	if (actual_task->parent != NULL)
		p = (pid_t)actual_task->parent->task_id;
	else
		p = 0;

	return p;
}

