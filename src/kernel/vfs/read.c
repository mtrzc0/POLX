#include <kernel/syscalls.h>
#include <kernel/klib.h>
#include <kernel/task.h>
#include <kernel/vmm.h>
#include <kernel/vfs.h>
#include <kernel/fd.h>
#include <errno.h>

extern int errno;
extern task_t *actual_task;

long do_read(task_t *t, int fd, void *buff, size_t size)
{
	fd_t *fd_ptr;
	char *src;
	long read_data;

	/* Bad file descriptor */
	if (t->used_fd[fd] == NULL) {
		errno = EBADF;
		return -1;
	}

	fd_ptr = t->used_fd[fd];

	/* Write only file descriptor */
	if (fd_ptr->mode & O_WRONLY) {
		errno = EPERM;
		return -1;
	}

	src = vmalloc(size, MAPPED);
	read_data = vfs_read(fd_ptr->vfs_node, fd_ptr->position, size, src);
	/* Failed to read, errno already set by lower level */
	if (read_data < 0) {
		vfree(src);
		return -1;
	}

	/* Failed to write data to userspace, errno already set */
	if (vmm_copy_data_to(t->aspace, (uintptr_t)buff, src, read_data) < 0) {
		vfree(src);
		return -1;
	}

	fd_ptr->position += read_data;
	vfree(src);
	
	return read_data;
}

long read(int fd, void *buff, size_t size)
{
	long ret;

	ret = do_read(actual_task, fd, buff, size);

	return ret;
}
