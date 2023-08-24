#include <kernel/syscalls.h>
#include <kernel/task.h>

extern task_t *actual_task;

pid_t getpid(void)
{
	return (pid_t)actual_task->task_id;
}

pid_t getppid(void)
{
	return (pid_t)actual_task->parent->task_id;
}

