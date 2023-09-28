#ifndef __devfs_dot_H
#define __devfs_dot_H

#include <kernel/vfs.h>

void devfs_init(void);

int devfs_add_dev(vfs_node_ptr_t dev);
vfs_node_ptr_t devfs_remove_dev(char *name);

/* Kernel pseudo devices */
vfs_node_ptr_t dev_zero_init(void);
vfs_node_ptr_t dev_stdout_init(void);
vfs_node_ptr_t dev_stdin_init(void);
void dev_stdin_recall(char *input_line, size_t size);

#endif
