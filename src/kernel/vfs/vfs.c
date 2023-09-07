#include <kernel/namei.h>
#include <kernel/klib.h>
#include <kernel/vfs.h>
#include <errno.h>

vfs_node_ptr_t tree_root;
extern int errno;

int vfs_open(vfs_node_ptr_t node, int mode)
{
	int ret = -1;

	if (node->op.open)
		ret = node->op.open(node, mode);

	if (ret < 0)
		errno = EACCES;
	else
		node->active_fds++;

	return ret;
}

int vfs_close(vfs_node_ptr_t node)
{
	int ret = -1;

	if (node->op.close)
		ret = node->op.close(node);
	
	if (ret < 0)
		errno = EBADF;
	else
		node->active_fds--;

	return ret;
}

long vfs_read(vfs_node_ptr_t node, size_t offset, size_t size, char *buffer)
{
	long ret = -1;

	if (node->op.read)
		ret = node->op.read(node, offset, size, buffer);
	
	if (ret < 0)
		errno = EINVAL;

	/* errno already set by driver */	
	if (ret == 0)
		ret = -1;

	return ret;
}

long vfs_write(vfs_node_ptr_t node, size_t offset, char *buffer, size_t size)
{
	long ret = -1;

	if (node->op.write)
		ret = node->op.write(node, offset, buffer, size);

	if (ret < 0)
		errno = EINVAL;

	return ret;
}

int vfs_touch(vfs_node_ptr_t p_node, vfs_node_ptr_t *node, char *name, int mode)
{
	int ret = -1;

	if (p_node->op.touch)
		ret = p_node->op.touch(p_node, node, name, mode);

	if (ret == 0)
		namei_add_child(p_node, *node);
	
	if (ret < 0)
		errno = EACCES;

	return ret;
}

int vfs_remove(vfs_node_ptr_t node)
{
	int ret = -1;

	if (node->op.remove)
		ret = node->op.remove(node);

	if (ret == 0) {
		namei_remove_child(node);
		kfree(node);
	}

	if (ret < 0)
		errno = EACCES;

	return ret;
}

int vfs_mkdir(vfs_node_ptr_t p_node, vfs_node_ptr_t *node, char *name, int mode)
{
	int ret = -1;

	if (p_node->op.mkdir)
		ret = p_node->op.mkdir(p_node, node, name, mode);

	if (ret == 0)
		namei_add_child(p_node, *node);

	if (ret < 0)
		errno = EACCES;


	return ret;
}

int vfs_rmdir(vfs_node_ptr_t node)
{
	int ret = -1;

	if (node->op.rmdir)
		ret = node->op.rmdir(node);

	if (ret == 0) {
		namei_remove_child(node);
		kfree(node);
	}

	if (ret < 0)
		errno = EACCES;

	return ret;
}

int vfs_readdir(vfs_node_ptr_t node, size_t no, vfs_dirent_t *dent)
{
	int ret = -1;

	if (node->op.readdir)
		ret = node->op.readdir(node, no, dent);

	if (ret < 0)
		errno = EACCES;

	return ret;
}
