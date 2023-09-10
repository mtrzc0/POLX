#include <kernel/syscalls.h>
#include <kernel/task.h>
#include <kernel/vmm.h>
#include <kernel/vfs.h>
#include <kernel/fd.h>
#include <errno.h>

extern vfs_node_ptr_t dev_zero;
extern task_t *actual_task;
extern int errno;

void *do_mmap(task_t *t, void *addr, size_t size, 
			int prot, int fd, size_t offset)
{
	void *mapping;
	uint32_t flags;
	fd_t *fd_ptr;

	if (t->used_fd[fd] == NULL) {
		errno = EBADF;
		return NULL;
	}

	/* File was not open as readable */
	fd_ptr = t->used_fd[fd];
	if (! (fd_ptr->mode & O_RDONLY || fd_ptr->mode & O_RDWR)) {
		errno = EACCES;
		return NULL;
	}

	/* File system does not support memory mapping */
	if (fd_ptr->vfs_node->v_type != VFS_FILE) {
		if (fd_ptr->vfs_node != dev_zero) {
			errno = ENODEV;
			return NULL;
		}
	}

	/* Translate prot into VMM flags */
	flags = VMM_USER;
	if (prot & PROT_WRITE)
		flags |= VMM_RW;

	mapping = vmm_mmap_at(t->aspace, (uintptr_t)addr, fd_ptr->vfs_node, 
							offset, size, 0, flags);
	
	return mapping;
}

void *mmap(void *addr, size_t size, int prot, int fd, size_t offset)
{
	return do_mmap(actual_task, addr, size, prot, fd, offset);
}
