#include <dev/terminal/terminal.h>
#include <kernel/klib.h>
#include <kernel/vfs.h>

vfs_node_ptr_t dev_stdout;

long _stdout_write(vfs_node_ptr_t node, size_t offset, char *buffer, size_t size)
{
	(void)node;
	size_t i;
	char *src;

	if (! is_terminal_initialized())
		terminal_init();

	src = (char *)(buffer + offset);
	for (i=0; i < size; i++) {
		terminal_putchar(src[i]);
	}

	return (long)i;
}

vfs_node_ptr_t dev_stdout_init(void)
{
	vfs_node_ptr_t vnode;

	vnode = (vfs_node_ptr_t)kmalloc(sizeof(vfs_node_t));
	memset(vnode, 0, sizeof(vfs_node_t));
	
	strncpy(vnode->name, "stdout", 7);
	vnode->v_type = VFS_CHRDEV;
	vnode->op.write = &_stdout_write;

	dev_stdout = vnode;

	return vnode;
}
