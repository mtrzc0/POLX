#ifndef __namei_dot_H
#define __namei_dot_H

#include <kernel_limits.h>
#include <kernel/vfs.h>

void namei_init(void);

vfs_node_ptr_t namei_path_to_vfs_node(vfs_node_ptr_t root, char *path, vfs_node_ptr_t *parent);

void namei_path_to_filename(const char *path, char *buff);

void namei_add_child(vfs_node_ptr_t parent, vfs_node_ptr_t child);

void namei_remove_child(vfs_node_ptr_t child);

int namei_mount(vfs_node_ptr_t mp, vfs_node_ptr_t fs_root);

#endif
