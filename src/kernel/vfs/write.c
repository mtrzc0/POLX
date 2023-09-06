#include <kernel/syscalls.h>
#include <kernel/task.h>
#include <kernel/vmm.h>
#include <kernel/vfs.h>
#include <kernel/fd.h>
#include <errno.h>

extern int errno;
extern task_t *actual_task;

long do_write(task_t *t, int fd, const void *buff, size_t size)
{
	uintptr_t paddr, old_tmp, current_vaddr;
	vfs_node_ptr_t node;
	long written, written_int;
	size_t write_int;
	char *src;

	/* FD does not exist */
	if (t->used_fd[fd] == NULL) {
		errno = EBADF;
		return -1; 
	}

	/* Read only FD */
	if (t->used_fd[fd]->mode & O_RDONLY) {
		errno = EPERM;
		return -1;
	}

	node = t->used_fd[fd]->vfs_node;

	/* Write data in page size chunks */
	written = 0;
	old_tmp = vmm_unmap(VM_TMP2_MAP);
	current_vaddr = (uintptr_t)buff;
	while ((size_t)written < size) {
		paddr = vmm_get_frame_from(t->aspace->pd, current_vaddr);
		if (! paddr) {
			errno = EFAULT;
			return -1;
		}
		vmm_map(paddr, VM_TMP2_MAP, PG_KERN_PAGE_FLAG);

		/* Buffer out of page boundary */
		if ((current_vaddr % PAGE_SIZE) + (size - written) > PAGE_SIZE) {
			write_int = PAGE_SIZE - (current_vaddr % PAGE_SIZE); 
		} else {
			write_int = size - written;
		}

		/* Write data to file */
		src = (char *)(VM_TMP2_MAP + (current_vaddr % PAGE_SIZE));
		written_int = vfs_write(node, t->used_fd[fd]->position, src, write_int);
		
		/* Something went wrong on lower level */
		if (written_int < 0) {
			vmm_unmap(VM_TMP2_MAP);
			vmm_map(old_tmp, VM_TMP2_MAP, PG_KERN_PAGE_FLAG);
			return -1;
		} else {
			written += written_int;
		}

		/* Update fd position */
		t->used_fd[fd]->position += written_int;

		current_vaddr += written_int;
		vmm_unmap(VM_TMP2_MAP);
	}

	vmm_map(old_tmp, VM_TMP2_MAP, PG_KERN_PAGE_FLAG);

	return written;
}

long write(int fd, const void *buff, size_t size)
{
	long ret;

	ret = do_write(actual_task, fd, buff, size);

	return ret;
}
