#ifndef __context_switch_dot_H
#define __context_switch_dot_H 

#include <i386/task_regs.h>

void context_switch(task_regs_t *regs, uintptr_t kern_stack);
void syscall_return(task_regs_t *regs);

#endif

