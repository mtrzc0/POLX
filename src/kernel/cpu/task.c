#include <kernel/klib.h>
#include <kernel/task.h>
#include <kernel/arch.h>
#include <kernel/signal.h>
#include <kernel/scheduler.h>

static tid_t old_tid = 0;
extern task_t *actual_task;
extern task_t *kernel_task;
task_t *task_tree_root = NULL;

static inline tid_t _get_tid(void)
{
	if (old_tid == MAX_TID)
		panic("[ERROR] End of tasks ID's!");

	old_tid++;
	return old_tid;
}

static void _add_child(task_t *parent, task_t *child)
{
	if (parent->first_child == NULL) {
		child->prev_sibling = child->next_sibling = NULL;
		parent->first_child = parent->last_child = child;
		return;
	}

	parent->last_child->next_sibling = child;
	child->prev_sibling = parent->last_child;
	child->next_sibling = NULL;
	parent->last_child = child;
}

static void _remove_child(task_t *child)
{
	task_t *parent;

	if (child->task_id == 1)
		panic("[ERROR] Tried to kill init task");

	parent = child->parent;
	
	if (child->prev_sibling == NULL) {
		parent->first_child = child->next_sibling;
		if (parent->first_child != NULL) {
			parent->first_child->prev_sibling = NULL;
			return;
		}
		// No return here, bc when child is alone we need to set
		// parent->last_child too
	}

	if (child->next_sibling == NULL) {
		parent->last_child = child->prev_sibling;
		if (parent->last_child != NULL)
			parent->last_child->next_sibling = NULL;
		return;
	}

	child->prev_sibling->next_sibling = child->next_sibling;
	child->next_sibling->prev_sibling = child->prev_sibling;
}

task_t *task_new(task_priority_t priority, vmm_aspace_t *aspace)
{
	task_t *new;

	new = (task_t *) kmalloc(sizeof(task_t));
	memset(new, 0, sizeof(task_t));
	new->task_id = _get_tid();
	new->kernel_stack = (uintptr_t) vmalloc(4096, MAPPED) + 0x1000;
	new->priority = priority;
	//new->regs.cr3 = vmm_create_directory();
	new->regs.cr3 = aspace->pd;
	new->aspace = aspace;

	/* Create root of tasks tree */
	if (task_tree_root == NULL) {
		new->parent = NULL;
		task_tree_root = new;
	} else {
		new->parent = actual_task;
		_add_child(actual_task, new);
	}

	return new;
}

void task_destroy(task_t *tcb)
{
	task_t *tmp, *tmp2;

	vfree((void *)(tcb->kernel_stack - 0x1000));
	vmm_aspace_destroy(tcb->aspace);
	vmm_destroy_directory(tcb->regs.cr3);

	/* Kill all task childs */
	tmp = tcb->first_child;	
	while (tmp != NULL) {
		tmp2 = tmp;
		tmp = tmp->next_sibling;
		task_destroy(tmp2);
	}
	//sched_remove(tcb);
	_remove_child(tcb);

	kfree(tcb);
}

void task_switch(void)
{
	task_t *next;

	/* 
	   Task switch can be preformed from 
	   task that was removed from CPU queue 
	*/
	if (actual_task->state == RUNNING)
		actual_task->state = READY;
	
	next = actual_task->next;	
	if (next == NULL) {
		next = sched_get_next_task();
		if (next == kernel_task)
			return;
	}
	
	next->state = RUNNING;
	
	sig_handler(next);
	actual_task = next;
	kprintf("[DEBUG] Task switch to %d\n", next->task_id);
	context_switch(&next->regs, next->kernel_stack);
}

void task_switch_to(task_t *task)
{
	if (actual_task->state == RUNNING)
		actual_task->state = READY;
	
	task->state = RUNNING;

	sig_handler(task);
	actual_task = task;
	kprintf("[DEBUG] Task force switch to %d\n", task->task_id);
	if (task->task_id == 0)
		panic("[DEBUG] End of all tasks");
	context_switch(&task->regs, task->kernel_stack);
}
