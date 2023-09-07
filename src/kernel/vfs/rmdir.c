#include <kernel/namei.h>
#include <kernel/klib.h>
#include <kernel/task.h>
#include <kernel/vfs.h>
#include <errno.h>

extern task_t *actual_task;
extern int errno;

int do_rmdir(char *path)
{
	vfs_node_ptr_t parent, dir;

	dir = namei_path_to_vfs_node(NULL, path, &parent);
	
	/* No such file or directory */
	if (dir == NULL) {
		errno = ENOENT;
		return -1;
	}

	/* Directory is being used as mount point */
	if (dir->v_type == VFS_MNTPT) {
		errno = EBUSY;
		return -1;
	}

	/* It is not a directory */
	if (dir->v_type != VFS_DIR) {
		errno = ENOTDIR;
		return -1;
	}

	/* Directory not empty */
	if (dir->first_child != NULL) {
		errno = EEXIST;
		return -1;
	}

	/* Failed to remove dir, errno already set */
	if (vfs_rmdir(dir) < 0)
		return -1;

	return 0;
}

int rmdir(char *path)
{
	int ret;
	char *path_in_kern_as;

	path_in_kern_as = vmm_copy_string_from(actual_task->aspace,
					(uintptr_t)path, MAX_PATH_LEN);

	if (path_in_kern_as == NULL)
		return -1;

	ret = do_rmdir(path_in_kern_as);
	kfree(path_in_kern_as);

	return ret;
}
