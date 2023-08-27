#include <kernel/klib.h>
#include <kernel/scheduler.h>

SCHEDULER_INFO sched_global;
task_t *actual_task;
task_t *kernel_task;

static task_t *next_to_run;

static queue_t os_q, user_q, sleep_q;

#define _init_queue(name)  \
	name.counter = 0;  \
	name.first = NULL; \
	name.last = NULL;

static inline queue_t *_set_queue(task_t *t)
{
	if (t->state == WAITING)
		return &sleep_q;
	else
		return (t->priority == OS) ? &os_q : &user_q;
}

static inline task_t *__find_in_queue(tid_t task_id, queue_t *q)
{
	task_t *tmp = q->first;

	while (tmp != NULL) {
		if (tmp->task_id == task_id) {
			return tmp;
		}
		tmp = tmp->next;
	}

	return NULL;
}

static void _add_to_queue(task_t *t)
{
	/* Task can be added only at the end of llist */
	queue_t *q;
	
	q = _set_queue(t);
	
	if (q->first == NULL) {
		q->first = t;
		t->prev = NULL;
	} else {
		t->prev = q->last;
		q->last->next = t;
	}

	t->next = NULL;
	q->last = t;
	q->counter++;
}

static void _remove_from_queue(task_t *t)
{
	queue_t *q;

	q = _set_queue(t);

	if (q->first == t) {
		q->first = t->next;
		if (q->first != NULL)
			q->first->prev = NULL; 
	} else if (q->last == t) {
		q->last = t->prev;
		q->last->next = NULL;
	} else {
		t->prev->next = t->next;
		t->next->prev = t->prev;
	}

	t->prev = NULL;
	t->next = NULL;
	q->counter--;
}

void sched_init(void)
{
	task_t *kern;

	_init_queue(os_q);
	_init_queue(user_q);
	_init_queue(sleep_q);

	sched_global.total_tasks = 0;
	sched_global.running_tasks = 0;
	sched_global.sleeping_tasks = 0;

	kern = kmalloc(sizeof(task_t));
	kern->task_id = 0;
	kern->priority = OS;
	kern->next = NULL;

	kernel_task = kern;
	actual_task = kernel_task;
	next_to_run = NULL;
}

task_t *sched_get_tcb(tid_t task_id)
{
	task_t *t;

	t = __find_in_queue(task_id, &sleep_q);
	if (t != NULL)
		return t;
	
	t = __find_in_queue(task_id, &os_q);
	if (t != NULL)
		return t;
	
	t = __find_in_queue(task_id, &user_q);
	if (t != NULL)
		return t;

	panic("[ERROR] Tried to use non-existent task");
	return 0;
}

task_t *sched_get_next_task(void)
{
	task_t *next;

	if (os_q.counter > 0) {
		if (next_to_run != NULL && next_to_run->priority == OS) {
			next = next_to_run;
			next_to_run = NULL;
		} else {
			next = os_q.first;
		}
	} else if (user_q.counter > 0) {
		if (next_to_run != NULL) {
			next = next_to_run;
			next_to_run = NULL;
		} else {
			next = user_q.first;
		}
	} else {
	/* No more task to do yet */
		next = kernel_task;
	}

	return next;
}

void sched_add_task(task_t *tcb)
{
	tcb->state = READY;
	sched_global.total_tasks++;
	sched_global.running_tasks++;

	_add_to_queue(tcb);
	
	/* Switch to higher priority task */
	if (tcb->priority > actual_task->priority) {
		next_to_run = actual_task->next;
		task_switch_to(tcb);
	}
}

void sched_remove(task_t *tcb)
{
	sched_global.total_tasks--;
	
	/* Zombie tasks are not queued */
	if (tcb->state == ZOMBIE)
		return;

	if (tcb->state == RUNNING || tcb->state == READY)
		sched_global.running_tasks--;
	else
		sched_global.sleeping_tasks--;

	_remove_from_queue(tcb);
}

void sched_set_state(tid_t task_id, bool mode)
{
	task_t *tcb;

	tcb = sched_get_tcb(task_id);

	/* Check if can preform task state change */	
	if ((tcb->state == READY || tcb->state == RUNNING) && mode)
		return;
	if (tcb->state == WAITING && mode == 0)
		return;

	_remove_from_queue(tcb);

	if (mode) {
		tcb->state = READY;
		sched_global.running_tasks++;
		sched_global.sleeping_tasks--; 
	} else {
		tcb->state = WAITING;
		sched_global.running_tasks--;
		sched_global.sleeping_tasks++; 
	}

	_add_to_queue(tcb);
	
	if (tcb->state == READY && 
		    tcb->priority > actual_task->priority) {
		next_to_run = actual_task->next;
		task_switch_to(tcb);
	}
}
