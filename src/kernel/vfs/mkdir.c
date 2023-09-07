#include <kernel/namei.h>
#include <kernel/klib.h>
#include <kernel/task.h>
#include <kernel/vfs.h>
#include <errno.h>

extern task_t *actual_task;
extern int errno;

int do_mkdir(char *path, int mode)
{
	vfs_node_ptr_t parent, dir;
	char dirname[MAX_FILENAME];

	dir = namei_path_to_vfs_node(NULL, path, &parent);

	/* File or directory exist */
	if (dir != NULL) {
		errno = EEXIST;
		return -1;
	}

	/* Path is not correct */
	if (parent == NULL) {
		errno = EINVAL;
		return -1;
	}

	/* Future parent it's not a directory */
	if (parent->v_type != VFS_DIR && parent->v_type != VFS_MNTPT) {
		errno = ENOTDIR;
		return -1;
	}

	namei_path_to_filename(path, &dirname[0]);

	if (vfs_mkdir(parent, &dir, &dirname[0], mode) < 0)
		return -1;

	return 0;
}

int mkdir(char *path, int mode)
{
	int ret;
	char *path_in_kern_as;

	path_in_kern_as = vmm_copy_string_from(actual_task->aspace,
					(uintptr_t)path, MAX_PATH_LEN);

	if (path_in_kern_as == NULL)
		return -1;

	ret = do_mkdir(path_in_kern_as, mode);
	kfree(path_in_kern_as);

	return ret;
}
