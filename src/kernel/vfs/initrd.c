#include <kernel/vfs.h>
#include <kernel/klib.h>
#include <kernel/tarfs.h>
#include <kernel/ramfs.h>
#include <kernel/namei.h>

extern vfs_node_ptr_t tree_root;

void initrd_load(uintptr_t start_of_initrd)
{
	vfs_node_ptr_t initrd_root;

	namei_init();

	initrd_root = tarfs_init(start_of_initrd);
	/* Check if unpacked properly */
	if (initrd_root == NULL)
		panic("Unrecognized initrd format or initrd is corrupted!");

	namei_mount(tree_root, initrd_root);
	
	/* Set previous op values of "/" node*/
	tree_root->op.open    = &ramfs_open;
	tree_root->op.close   = &ramfs_close;
	tree_root->op.touch   = &ramfs_touch;
	tree_root->op.remove  = &ramfs_remove;
	tree_root->op.mkdir   = &ramfs_mkdir;
	tree_root->op.readdir = &ramfs_readdir;

	/* Free unnessesary vnode when fs is mounted */
	kfree(initrd_root);
}
