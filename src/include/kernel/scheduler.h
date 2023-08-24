#ifndef __scheduler_dot_H
#define __scheduler_dot_H

#include <stddef.h>
#include <stdbool.h>
#include <kernel/task.h>

typedef struct {
	size_t total_tasks;
	size_t running_tasks;
	size_t sleeping_tasks;
} SCHEDULER_INFO;

struct queue {
	size_t counter;
	task_t *first, *last;
};
typedef struct queue queue_t; 

void sched_init(void);
task_t *sched_get_tcb(tid_t task_id);
task_t *sched_get_next_task(void);
void sched_add_task(task_t *t);
void sched_remove(task_t *tcb);
void sched_set_state(tid_t task_id, bool mode);

#define sched_wake_task(task_id) \
	sched_set_state(task_id, 1)

#define sched_sleep_task(task_id) \
	sched_set_state(task_id, 0)

#endif
