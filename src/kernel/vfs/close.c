#include <kernel/klib.h>
#include <kernel/task.h>
#include <kernel/vfs.h>
#include <kernel/fd.h>

#include <errno.h>

extern int errno;
extern task_t *actual_task;

int do_close(task_t *t, int fd)
{
	fd_t *fd_ptr;

	/* fd does not exist */
	if (t->used_fd[fd] == NULL) {
		errno = EBADF;
		return -1;
	}
	fd_ptr = t->used_fd[fd];

	if (vfs_close(fd_ptr->vfs_node) < 0)
		return -1;
	
	fd_remove(fd_ptr);
	t->used_fd[fd] = NULL;
	t->fd_ctr--;
	kfree(fd_ptr);

	return 0;
}

int close(int fd)
{
	return do_close(actual_task, fd);
}
