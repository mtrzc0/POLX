#include <kernel/syscalls.h>
#include <kernel/klib.h>
#include <kernel/namei.h>
#include <kernel/task.h>
#include <kernel/vfs.h>
#include <kernel/vmm.h>
#include <kernel/fd.h>

#include <errno.h>

#define _IS_FLG(mask, flag) ((mask) & (flag))

extern int errno;
extern task_t *actual_task;

static int _find_free_fd_num(fd_t **fd_table)
{
	int i = 2;
	while ((fd_table[i]) != NULL && i < MAX_FDS)
		i++;

	/* Free fd number or error */
	return (fd_table[i] == NULL) ? i : -1;
}

int do_open(task_t *t, char *path, int flags)
{
	vfs_node_ptr_t file_parent, file;
	char filename[MAX_FILENAME];
	fd_t *new_fd;
	int fd_num;

	file = namei_path_to_vfs_node(NULL, path, &file_parent);
	/* File does not exist */
	if (file == NULL) {
		/* Wrong path */
		if (file_parent == NULL) {
			errno = ENOENT;
			return -1;
		}

		/* Create file */
		if (_IS_FLG(flags, O_CREAT)) {
			namei_path_to_filename(path, filename);
			if (vfs_touch(file_parent, &file, filename, flags) < 0)
				return -1;
		} else {
			errno = ENOENT;
			return -1;
		}
	}

	if (vfs_open(file, flags) < 0)
		return -1;

	/* Create new file descriptor */
	new_fd = kmalloc(sizeof(fd_t));
	new_fd->position = 0;
	new_fd->mode = flags;
	new_fd->vfs_node = file;
	fd_add(new_fd);

	/* Update TCB */
	fd_num = _find_free_fd_num(t->used_fd);
	/* No more free fd numbers */
	if (fd_num < 0) {
		errno = EMFILE;
		return -1;
	}
	t->used_fd[fd_num] = new_fd;
	t->fd_ctr++;

	return fd_num;
}

int open(char *path, int flags)
{
	int ret;
	char *path_in_kernel_as;
	
	path_in_kernel_as = vmm_copy_string_from(actual_task->aspace, 
					(uintptr_t)path, MAX_PATH_LEN);
	if (path_in_kernel_as == NULL)
		return -1;
	
	ret = do_open(actual_task, path_in_kernel_as, flags);
	kfree(path_in_kernel_as);

	return ret;
}
