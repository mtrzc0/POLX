#include <kernel/syscalls.h>
#include <kernel/task.h>
#include <kernel/vfs.h>
#include <kernel/vmm.h>
#include <kernel/fd.h>
#include <errno.h>

extern task_t *actual_task;
extern int errno;

int do_readdir(task_t *t, int fd, vfs_dirent_t *dent)
{
	vfs_dirent_t kern_dent;
	fd_t *fd_ptr;
	int ret;

	/* File descriptor does not exist */
	if (t->used_fd[fd] == NULL) {
		errno = EBADF;
		return -1;
	}
	
	fd_ptr = t->used_fd[fd];

	/* Not a directory */
	if (fd_ptr->vfs_node->v_type != VFS_DIR && 
	     fd_ptr->vfs_node->v_type != VFS_MNTPT) {
		errno = ENOTDIR;
		return -1;
	}

	ret = vfs_readdir(fd_ptr->vfs_node, fd_ptr->position, &kern_dent);
	
	/* Errno already set */
	if (ret < 0)
		return -1;

	/* End of directory */
	if (ret == 0)
		return 0;

	if (vmm_copy_data_to(t->aspace, (uintptr_t)dent, &kern_dent,
					sizeof(vfs_dirent_t)) < 0)
		return -1;

	fd_ptr->position++;

	return 1;
}

int readdir(int fd, vfs_dirent_t *dent)
{
	return do_readdir(actual_task, fd, dent);
}
