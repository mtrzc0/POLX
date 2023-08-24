#ifndef __ramfs_dot_H
#define __ramfs_dot_H

#include <kernel/vfs.h>

struct ramfs_inode {
	uintptr_t paddr; /* Address of frame holding data */
};
typedef struct ramfs_inode ramfs_inode_t;

int ramfs_open(vfs_node_ptr_t node, int mode);
int ramfs_close(vfs_node_ptr_t node);
long ramfs_read(vfs_node_ptr_t node, size_t offset, size_t size, char *buffer);
long ramfs_write(vfs_node_ptr_t node, size_t offset, char *buffer, size_t size);
int ramfs_touch(vfs_node_ptr_t p_node, vfs_node_ptr_t *node, char *name, int mode);
int ramfs_remove(vfs_node_ptr_t node);
int ramfs_mkdir(vfs_node_ptr_t p_node, vfs_node_ptr_t *node, char *name, int mode);
int ramfs_rmdir(vfs_node_ptr_t node);
int ramfs_readdir(vfs_node_ptr_t node, size_t no, vfs_dirent_t *dent);

#endif
