#include <errno.h>
#include <kernel/klib.h>
#include <kernel/tarfs.h>
#include <kernel/namei.h>
#include <kernel/vfs.h>

extern int errno;

vfs_node_ptr_t tar_tree;

static size_t _get_size(const char *s)
{
	size_t size, count;

	size = 0;
	count = 1;

	for (size_t i=11; i > 0; i--) {
		size += ((s[i-1] - 48) * count);
		count *= 8;
	}

	return size;
}

static vfs_node_ptr_t _init_vnode(tar_hdr_t *ptr, size_t fsize)
{
	char filename[MAX_FILENAME];
	vfs_node_ptr_t vnode;
	tar_inode_t *inode;

	vnode = (vfs_node_ptr_t) kmalloc(sizeof(vfs_node_t));
	inode = (tar_inode_t *) kmalloc(sizeof(tar_inode_t));
	memset(vnode, 0, sizeof(vfs_node_t));
	memset(inode, 0, sizeof(tar_inode_t));

	/* Fill inode */
	inode->vaddr = (uintptr_t) ((uintptr_t)ptr+512);
	inode->hdr = ptr;
	
	/* Fill vnode */
	namei_path_to_filename(ptr->filename, filename);
	strncpy(vnode->name, filename, TAR_MAX_FILENAME);
	vnode->size = fsize - 512;

	switch(ptr->typeflag) {
	case TAR_REG_FILE:
	case TAR_REG_FILE2:
		vnode->v_type = VFS_FILE;
		vnode->op.open = &tarfs_open;
		vnode->op.close = &tarfs_close;
		vnode->op.read = &tarfs_read;
		break;
	
	case TAR_DIR:
		vnode->v_type = VFS_DIR;
		vnode->op.open = &tarfs_open;
		vnode->op.close = &tarfs_close;
		vnode->op.readdir = &tarfs_readdir;
		break;
	default:
		kprintf("Type of %s file is not known!");
		panic("");
	};

	vnode->data = (void *)inode;

	return vnode;
}

vfs_node_ptr_t tarfs_init(uintptr_t start_of_archive)
{
	size_t size;
	tar_hdr_t *hdr_ptr;
	vfs_node_ptr_t root, parent, tmp;

	/* Init root of tarfs tree */
	root = (vfs_node_ptr_t) kmalloc(sizeof(vfs_node_t));
	memset(root, 0, sizeof(vfs_node_t));
	strncpy(root->name, "/", 2);
	root->v_type = VFS_DIR;
	root->op.readdir = &tarfs_readdir;

	while (1) {
		hdr_ptr = (tar_hdr_t *)start_of_archive;
		if (hdr_ptr->filename[0] == '\0')
			break;

		size = _get_size(hdr_ptr->size);
		size = (size % 512) ? size+512 : size;

		/* Create tarfs tree */
		tmp = _init_vnode(hdr_ptr, size);
		namei_path_to_vfs_node(root, &hdr_ptr->filename[1], &parent);
			
		/* Corrupted tar archive */
		if (parent == NULL) {
			return NULL;
		}

		namei_add_child(parent, tmp);

		/* Calculate next header address */
		start_of_archive += ((size / 512) + 1) * 512;
	}

	return root;
}

int tarfs_open(vfs_node_ptr_t node, int mode)
{
	(void)node;
	(void)mode; 
	return 0;
}

int tarfs_close(vfs_node_ptr_t node)
{
	(void)node;
	return 0;
}

long tarfs_read(vfs_node_ptr_t node, size_t offset, size_t size, char *buffer)
{
	tar_inode_t *inode;
	size_t read_data;
	char *src_arr;

	inode = (tar_inode_t *)node->data;
	src_arr = (char *)(inode->vaddr + offset);
	read_data = 0;
	while (read_data < size && node->size-offset-read_data > 0) {
		buffer[read_data] = src_arr[read_data];
		read_data++;
	}

	if (read_data == 0)
		errno = EIO;	
	
	return (long)read_data;
}

int tarfs_readdir(vfs_node_ptr_t node, size_t no, vfs_dirent_t *dent)
{
	size_t i;
	vfs_node_ptr_t child;

	i = 0;
	child = node->first_child;
	while (child != NULL && i <= no) {
		if (i == no) {
			strncpy(dent->name, child->name, MAX_FILENAME);
			dent->ino = child->ino;
			return 1;
		}
		i++;
		child = child->next_sibling;
	}

	return 0;
}
