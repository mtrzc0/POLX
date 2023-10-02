#ifndef __vfs_dot_H
#define __vfs_dot_H

#include <stdint.h>
#include <stddef.h>
#include <kernel_limits.h>

typedef struct vfs_node * vfs_node_ptr_t;

typedef enum {
	VFS_FILE = 1,
	VFS_DIR,
	VFS_CHRDEV,
	VFS_BLKDEV,
	VFS_PIPE,
	VFS_SYMLINK,
	VFS_MNTPT
} vfs_type;

struct vfs_dir_entry {
	char name[MAX_FILENAME];
	size_t ino;
};
typedef struct vfs_dir_entry vfs_dirent_t;

struct vfs_operations {
	int (*open) (vfs_node_ptr_t, int);
	int (*close) (vfs_node_ptr_t);
	long (*read) (vfs_node_ptr_t, size_t, size_t, char *);
	long (*write) (vfs_node_ptr_t, size_t, char *, size_t);
	int (*touch) (vfs_node_ptr_t, vfs_node_ptr_t *,char *, int);
	int (*remove) (vfs_node_ptr_t);
	int (*mkdir) (vfs_node_ptr_t, vfs_node_ptr_t *,char *, int);
	int (*rmdir) (vfs_node_ptr_t);
	int (*readdir) (vfs_node_ptr_t, size_t, vfs_dirent_t *);
};
typedef struct vfs_operations vfs_ops_t;

struct vfs_node {
	char name[MAX_FILENAME]; /* Filename */
	// Place for permissions, owner etc
	vfs_type v_type;	 /* Type of file */
	size_t ino;		 /* Unique number used by filesystem */
	size_t size;		 /* Size of file */
	size_t active_fds;	 /* Active file descriptors counter */
	vfs_ops_t op;		 /* Pointers to possible operations on file */

	/* Virtual filesystem tree */
	vfs_node_ptr_t parent;
	vfs_node_ptr_t first_child, last_child;
	vfs_node_ptr_t prev_sibling, next_sibling;

	void *data;		 /* Private fs data */
};
typedef struct vfs_node vfs_node_t;

int vfs_open(vfs_node_ptr_t node, int mode);
int vfs_close(vfs_node_ptr_t node);
long vfs_read(vfs_node_ptr_t node, size_t offset, size_t size, char *buffer);
long vfs_write(vfs_node_ptr_t node, size_t offset, char *buffer, size_t size);
int vfs_touch(vfs_node_ptr_t p_node, vfs_node_ptr_t *node, char *name, int mode);
int vfs_remove(vfs_node_ptr_t node);
int vfs_mkdir(vfs_node_ptr_t p_node, vfs_node_ptr_t *node, char *name, int mode);
int vfs_rmdir(vfs_node_ptr_t node);
int vfs_readdir(vfs_node_ptr_t node, size_t no, vfs_dirent_t *dent);

#endif 
