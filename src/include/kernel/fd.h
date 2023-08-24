#ifndef __fd_dot_H
#define __fd_dot_H

#include <stddef.h>
#include <kernel/vfs.h>

struct file_descriptor {
	size_t position;
	int mode;
	vfs_node_ptr_t vfs_node;

	struct file_descriptor *next, *prev;
};
typedef struct file_descriptor fd_t;

void fd_add(fd_t *fd);
void fd_remove(fd_t *fd);

#endif
