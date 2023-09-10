#include <kernel/task.h>
#include <kernel/vmm.h>

extern task_t *actual_task;

int do_munmap(task_t *t, void *addr)
{
	return vmm_munmap(t->aspace, (uintptr_t)addr);
}

int munmap(void *addr)
{
	return do_munmap(actual_task, addr);
}
