#ifndef __task_dot_H
#define __task_dot_H

#include <stddef.h>

#include <kernel/arch.h>
#include <kernel/vmm.h>
#include <kernel/fd.h>

#ifdef i386
typedef uint32_t tid_t;
#define MAX_TID UINT32_MAX
#endif

#define MAX_FDS 256

typedef enum { 
	READY,		/* Ready for get a CPU time */
	RUNNING,	/* Already running */
	WAITING, 	/* Waiting for event */
	ZOMBIE		/* Inactive, waiting for parent attention */
} task_state_t;

typedef enum {
	USER,		/* User programs (lowest priority) */
	OS		/* OS programs (highest priority) */
} task_priority_t;

typedef uint32_t sig_t;

struct tcb {
	/* General informations */
	tid_t task_id;
	uintptr_t kernel_stack;
	int exit_code;
	
	/* File descriptors */
	size_t fd_ctr;
	fd_t *used_fd[MAX_FDS];
	
	/* Signals */
	sig_t mask;
	sig_t current;

	/* waitpid syscall */
	struct tcb *wait4;

	/* Task tree structure */
	struct tcb *parent;
	struct tcb *first_child, *last_child;
	struct tcb *prev_sibling, *next_sibling;

	/* CPU scheduler info */
	task_state_t state;
	task_priority_t priority;
	struct tcb *next, *prev;

	/* Task context */
	task_regs_t regs;	/* CPU registers */
	vmm_aspace_t *aspace;	/* AS abstraction */
};
typedef struct tcb task_t;

task_t *task_new(task_priority_t priority, vmm_aspace_t *aspace);
void task_destroy(task_t *tcb);
void task_switch(void);
void task_switch_to(task_t *task);

#endif
