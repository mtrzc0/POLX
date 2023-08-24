#include <kernel/klib.h>
#include <kernel/namei.h>
#include <kernel/devfs.h>

extern vfs_node_ptr_t dev_zero;
extern vfs_node_ptr_t dev_stdout;

static vfs_node_ptr_t devdir;

void _init_pseudo_devs(void);

void devfs_init(void)
{
	vfs_node_ptr_t parent;

	devdir = namei_path_to_vfs_node(NULL, "/dev", &parent);

	/* Create dev dir */
	if (devdir != NULL) {
		panic("[ERROR] /dev already exist");
	} else if (devdir == NULL && parent != NULL) {
		if (vfs_mkdir(parent, &devdir, "dev", 0))
			panic("[ERROR] Failed to create directory for devfs");
	} else {
		panic("[ERROR] Failed to create directory for devfs");
	}

	_init_pseudo_devs();
}

int devfs_add_dev(vfs_node_ptr_t dev)
{
	vfs_node_ptr_t tmp;

	tmp = devdir->first_child;
	while (tmp) {
		if (!strcmp(tmp->name, dev->name))
			return -1;
		tmp = tmp->next_sibling;
	}

	namei_add_child(devdir, dev);
	return 0;
}

vfs_node_ptr_t devfs_remove_dev(char *name)
{
	vfs_node_ptr_t tmp;

	tmp = devdir->first_child;
	while (tmp) {
		if (strcmp(tmp->name, name))
			break;
		tmp = tmp->next_sibling;
	}

	if (tmp != NULL)
		namei_remove_child(tmp);

	return tmp;
}


void _init_pseudo_devs(void)
{
	namei_add_child(devdir, dev_zero_init());
	namei_add_child(devdir, dev_stdout_init());
}
