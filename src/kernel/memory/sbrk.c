#include <kernel/task.h>
#include <kernel/vmm.h>

extern task_t *actual_task;

void *do_sbrk(task_t *t, intptr_t inc)
{
	uintptr_t vaddr;

	if (inc == 0)
		return (void *)t->aspace->data_end;

	vaddr = t->aspace->data_end;
	vaddr += inc;

	return vmm_set_brk(t->aspace, vaddr);
}

void *sbrk(intptr_t inc)
{
	return do_sbrk(actual_task, inc);
}
