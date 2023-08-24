#ifndef __tarfs_dot_H
#define __tarfs_dot_H

#include <kernel/vfs.h>

#define TAR_MAX_FILENAME 100
#define TAR_REG_FILE 	'0'
#define TAR_REG_FILE2 	'\0'
#define TAR_DIR		'5'

struct tar_header {
	char filename[TAR_MAX_FILENAME];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char chksum[8];
	char typeflag;
};
typedef struct tar_header tar_hdr_t;

struct tar_inode {
	uintptr_t vaddr;
	tar_hdr_t *hdr;
};
typedef struct tar_inode tar_inode_t;

/* 
   Create independent vfs_tree of archive
   Return its root 
*/
vfs_node_ptr_t tarfs_init(uintptr_t start_of_archive);

int tarfs_open(vfs_node_ptr_t node, int mode);
int tarfs_close(vfs_node_ptr_t node);
long tarfs_read(vfs_node_ptr_t node, size_t offset, size_t size, char *buffer);
int tarfs_readdir(vfs_node_ptr_t node, size_t no, vfs_dirent_t *dent);

#endif
