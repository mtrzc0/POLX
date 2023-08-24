#include <kernel/klib.h>
#include <kernel/vfs.h>

vfs_node_ptr_t dev_zero;

long _zero_read(vfs_node_ptr_t node, size_t offset, size_t size, char *buffer)
{
	(void)node;
	(void)offset;
	
	memset(buffer, 0, size);

	return size;
}

long _zero_write(vfs_node_ptr_t node, size_t offset, char *buffer, size_t size)
{
	(void)node;
	(void)offset;
	(void)buffer;

	return size;
}

vfs_node_ptr_t dev_zero_init(void)
{
	vfs_node_ptr_t vnode;

	vnode = (vfs_node_ptr_t)kmalloc(sizeof(vfs_node_t));
	memset(vnode, 0, sizeof(vfs_node_t));
	
	strncpy(vnode->name, "zero", 5);
	vnode->v_type = VFS_CHRDEV;
	vnode->op.read = &_zero_read;
	vnode->op.write = &_zero_write;

	dev_zero = vnode;
	return vnode;
}
