# ***Divide CPU by all tasks***
Table of contents:
0. [Strategy](#scheduling-strategy)   
1. [Used data structures](#used-data-structures)   

2. API
    - [Init scheduler](#initialization)   
    - [Get tcb](#get-tcb)   
    - [Get next task](#get-next-task)   
    - [Add task to queue](#add-task-to-queue)   
    - [Remove task from queue](#remove-task-from-queue)   
    - [Change task state](#change-task-state)

# Scheduling strategy
**Priority-Based Round Robin**   
Tasks are divided into two types based on their destination:   
- OS task - usually an os server i.e device manager   
- User task - userspace program i.e shell   

This breakdown gives ability to prioritize tasks in the schuduling queue. Preemptive scheduling based on timer and priority guarantee that higher proirity tasks will be served first, but when all tasks have the same priority then they will be handled according to the Round Robin model.    

# _<kernel/scheduler.h>_

## Global data
```c
typedef struct {
	size_t total_tasks;
	size_t running_tasks;
	size_t sleeping_tasks;
} SCHEDULER_INFO;

task_t *actual_task;
task_t *kernel_task;
```

## Used data structures
```c
typedef enum {
	READY,		/* Ready for get a CPU time */
	RUNNING,	/* Already running */
	WAITING, 	/* Waiting for event */
    ZOMBIE      /* Inactive, waiting for parent attention */
} task_state_t;

typedef enum {
	USER,		/* User programs (lowest priority) */
    OS  		/* Os programs (highest priority) */
} task_priority_t;

struct queue {
	size_t counter;
	task_t *first, *last;
};
typedef struct queue queue_t;
```

## API
### Initialization
```c
void sched_init(void);
```
Init internal data structures and create kernel task.

### Get tcb
```c
task_t *sched_get_tcb(tid_t task_id);
```
Return tcb of task_id

### Get next task
```c
task_t *sched_get_next_task(void);
```
Called at the end of the tasks queue

### Add task to queue
```c
void sched_add_task(task_t *t);
```
Insert provided task into appropriate queue. If inserted task have higher priority swich to it.

### Remove task from queue
```c
void sched_remove(task_t *tcb);
```
Remove task from any queue

### Change task state
```c
void sched_set_state(tid_t task_id, bool mode);

#define sched_wake_task(task_id) \
	sched_set_state(task_id, 1)

#define sched_sleep_task(task_id) \
	sched_set_state(task_id, 0)
```
