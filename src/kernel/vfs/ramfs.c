#include <errno.h>
#include <kernel/klib.h>
#include <kernel/arch.h>
#include <kernel/ramfs.h>
#include <kernel/vfs.h>
#include <kernel/vmm.h>
#include <kernel/pmm.h>

extern int errno;

int ramfs_open(vfs_node_ptr_t node, int mode)
{
	(void)node;
	(void)mode;
	return 0;
}

int ramfs_close(vfs_node_ptr_t node)
{
	(void)node;
	return 0;
}

long ramfs_read(vfs_node_ptr_t node, size_t offset, size_t size, char *buffer)
{
	ramfs_inode_t *inode;
	size_t read_data;
	uintptr_t src;
	char *src_arr;

	/* Find free vaddr and map ramfs file */
	inode = (ramfs_inode_t *)node->data;
	src = (uintptr_t)vmalloc(4096, ON_DEMAND);
	vmm_map(inode->paddr, src, PG_KERN_PAGE_RO_FLAG);

	/* Write data from ramfs file to buffer */
	src_arr = (char *)(src+offset);
	read_data = 0;
	while (read_data < size && node->size-offset-read_data > 0) {
		buffer[read_data] = src_arr[read_data];
		read_data++;
	}

	vmm_unmap(src);
	vfree((void *)src);

	if (read_data == 0)
		errno = EIO;

	return (long)read_data;
}

long ramfs_write(vfs_node_ptr_t node, size_t offset, char *buffer, size_t size)
{
	ramfs_inode_t *inode;
	size_t written_data;
	uintptr_t dst;
	char *dst_arr;

	/* Find free vaddr and map ramfs file */
	inode = (ramfs_inode_t *)node->data;
	dst = (uintptr_t)vmalloc(4096, ON_DEMAND);
	vmm_map(inode->paddr, dst, PG_KERN_PAGE_FLAG);

	/* Write data from buffer to ramfs file */
	dst_arr = (char *)(dst+offset);
	written_data = 0;
	while (written_data < size && node->size-offset-written_data > 0) {
		dst_arr[written_data] = buffer[written_data];
		written_data++;
	}

	vmm_unmap(dst);
	vfree((void *)dst);

	if (written_data == 0)
		errno = EIO;

	return (long)written_data;
}

int ramfs_touch(vfs_node_ptr_t p_node, vfs_node_ptr_t *node, char *name, int mode)
{
	(void)p_node;
	(void)mode;
	ramfs_inode_t *inode;
	vfs_node_ptr_t vnode;

	inode = kmalloc(sizeof(ramfs_inode_t));
	vnode = kmalloc(sizeof(vfs_node_t));
	memset(vnode, 0, sizeof(vfs_node_t));

	inode->paddr = pmm_get_frame();

	/* Setup vnode struct */
	strncpy(vnode->name, name, MAX_FILENAME);
	vnode->size = 4096;
	vnode->v_type = VFS_FILE;
	vnode->op.open = &ramfs_open;
	vnode->op.close = &ramfs_close;
	vnode->op.read = &ramfs_read;
	vnode->op.write = &ramfs_write;
	vnode->op.remove = &ramfs_remove;
	vnode->data = inode;	
	*node = vnode;

	return 0;
}

int ramfs_remove(vfs_node_ptr_t node)
{
	ramfs_inode_t *inode;

	inode = (ramfs_inode_t *)node->data;
	pmm_free_frame(inode->paddr);
	kfree(inode);
	node->data = NULL;

	return 0; 
}

int ramfs_mkdir(vfs_node_ptr_t p_node, vfs_node_ptr_t *node, char *name, int mode)
{
	(void)p_node;
	(void)mode;

	vfs_node_ptr_t vnode;

	vnode = kmalloc(sizeof(vfs_node_t));
	memset(vnode, 0, sizeof(vfs_node_t));

	strncpy(vnode->name, name, MAX_FILENAME);
	vnode->v_type = VFS_DIR;
	vnode->op.touch = &ramfs_touch;
	vnode->op.mkdir = &ramfs_mkdir;
	vnode->op.rmdir = &ramfs_rmdir;
	vnode->op.readdir = &ramfs_readdir;
	*node = vnode;

	return 0;
}

int ramfs_rmdir(vfs_node_ptr_t node)
{
	(void)node;
	return 0;
}

int ramfs_readdir(vfs_node_ptr_t node, size_t no, vfs_dirent_t *dent)
{
	size_t i;
	vfs_node_ptr_t child;

	i = 0;
	child = node->first_child;
	while (child != NULL && i <= no) {
		if (i == no) {
			strncpy(dent->name, child->name, MAX_FILENAME);
			dent->ino = child->ino;
			return 0;
		}
		i++;
		child = child->next_sibling;
	}

	return -1;
}
