#include <kernel/namei.h>
#include <kernel/klib.h>
#include <kernel/task.h>
#include <kernel/vfs.h>
#include <kernel/vmm.h>
#include <errno.h>

extern task_t *actual_task;
extern int errno;

int do_remove(char *path)
{
	vfs_node_ptr_t parent, file;
	
	file = namei_path_to_vfs_node(NULL, path, &parent);
	/* File not found */
	if (file == NULL) {
		errno = ENOENT;
		return -1;
	}

	/* It is a directory use rmdir instead */
	if (file->v_type == VFS_DIR) {
		errno = EISDIR;
		return -1;
	}

	/* File is being used by other tasks */
	if (file->active_fds > 0) {
		errno = EBUSY;
		return -1;
	}

	/* Failed to remove file, errno already set */
	if (vfs_remove(file) < 0)
		return -1;

	return 0;
}

int remove(const char *path)
{
	int ret;
	char *path_in_kern_as;

	path_in_kern_as = vmm_copy_string_from(actual_task->aspace,
					(uintptr_t)path, MAX_PATH_LEN);

	if (path_in_kern_as == NULL)
		return -1;

	ret = do_remove(path_in_kern_as);
	kfree(path_in_kern_as);

	return ret;
}
