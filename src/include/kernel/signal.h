#ifndef __signal_dot_H
#define __signal_dot_H

#include <stdint.h>
#include <kernel/task.h>

/* Implemented signals */
#define SIGKILL 1
#define SIGSEGV 2
#define SIGCHLD 4

#define SIG_DEFAULT_MASK SIGKILL | SIGSEGV

#define SIG_CHECK_MASK(mask, sig) ((mask) & (sig))
#define SIG_MARK(mask, sig)       ((mask) |= (sig))
#define SIG_UNMARK(mask, sig)     ((mask) ^= (sig))

int sig_send(task_t *dst, sig_t signal);
void sig_handler(task_t *t);

#endif
