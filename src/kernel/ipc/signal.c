#include <errno.h>

#include <kernel/syscalls.h>
#include <kernel/signal.h>
#include <kernel/task.h>
#include <kernel/arch.h>

extern int errno;

int sig_send(task_t *dst, sig_t signal)
{
	/* Reject not wanted signals */
	if (!SIG_CHECK_MASK(dst->mask, signal))
		return 0;
	
	/* Mark signals */
	switch(signal) {
	case SIGKILL:
		do_exit(dst, 143);
		break;
	case SIGSEGV:
		do_exit(dst, 139);
		break;
	case SIGCHLD:
		SIG_MARK(dst->current, signal);
		break;
	default:
		errno = EINVAL;
		return -1;
	}

	return 0;
}

void sig_handler(task_t *t)
{
	if (SIG_CHECK_MASK(t->current, SIGCHLD)) {
		/* Return value from waitpid syscall */
		regs_set_retval(t->regs, t->wait4->exit_code);
		task_destroy(t->wait4);
		SIG_UNMARK(t->mask, SIGCHLD);
		SIG_UNMARK(t->current, SIGCHLD);
		return;
	}
}
