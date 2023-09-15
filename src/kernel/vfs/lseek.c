#include <kernel/syscalls.h>
#include <kernel/task.h>
#include <kernel/vfs.h>
#include <kernel/fd.h>
#include <errno.h>

extern task_t *actual_task;
extern int errno;

long do_lseek(task_t *t, int fd, long offset, int whence)
{
	fd_t *fd_ptr;
	size_t new_offset;

	/* Fd does not exist */
	if (t->used_fd[fd] == NULL) {
		errno = EBADF;
		return -1;
	}

	fd_ptr = t->used_fd[fd];
	
	/* Fd points to object that offset cannot be changed */
	if (fd_ptr->vfs_node->v_type != VFS_FILE) {
		errno = ESPIPE;
		return -1;
	}

	switch (whence) {
	case SEEK_SET:
	case SEEK_END:
		/* Offset points beyond the EOF */
		if (offset < 0 || (size_t)offset > fd_ptr->vfs_node->size) {
			errno = ENXIO;
			return -1;
		}
		fd_ptr->position = offset;
		break;

	case SEEK_CUR:
		new_offset = fd_ptr->position + offset;
		if (new_offset > fd_ptr->vfs_node->size) {
			errno = ENXIO;
			return -1;
		}
		fd_ptr->position = new_offset;
		break;

	default:
		errno = EINVAL;
		return -1;
	}

	return fd_ptr->position;
}

long lseek(int fd, long offset, int whence)
{
	return do_lseek(actual_task, fd, offset, whence);
}
