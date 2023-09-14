#include <kernel/task.h>
#include <kernel/vmm.h>

extern task_t *actual_task;

int do_brk(task_t *t, void *addr)
{
	void *ret;

	ret = vmm_set_brk(t->aspace, (uintptr_t)addr);

	if (ret == NULL)
		return -1;

	return 0;
}

int brk(void *addr)
{
	return do_brk(actual_task, addr);
}
