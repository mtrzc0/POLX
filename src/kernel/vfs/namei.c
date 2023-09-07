#include <kernel/klib.h>
#include <kernel/ramfs.h>
#include <kernel/vfs.h>

extern vfs_node_ptr_t tree_root;

static vfs_node_ptr_t _find_child(vfs_node_ptr_t parent, char *name)
{
	vfs_node_ptr_t tmp;
	
	tmp = parent->first_child;
	while (tmp != NULL) {
		if (strcmp(tmp->name, name) == 0)
			return tmp;
		tmp = tmp->next_sibling;
	}

	return NULL;
}

static size_t _path_deconcatenator(const char *path, size_t path_len, size_t prev_idx, char *buff)
{
	size_t buff_eof;

	buff_eof = 0;
	while (prev_idx < path_len) {
		if (path[prev_idx] == '/' || path[prev_idx] == '\0') {
			buff[buff_eof] = '\0';	
			prev_idx++;
			break;
		}

		/* Buffer overflow prevention */
		if (buff_eof == MAX_FILENAME) {
			buff[MAX_FILENAME-1] = '\0';
			return 0;
		}

		buff[buff_eof] = path[prev_idx];
		buff_eof++;
		prev_idx++;
	}

	/* For proper handling paths ended with '/' */
	if (path[prev_idx-1] == '/' && path[prev_idx] == '\0')
		prev_idx++;

	return (prev_idx == path_len) ? 0 : prev_idx;
}

void namei_init(void)
{
	tree_root = (vfs_node_ptr_t) kmalloc(sizeof(vfs_node_t));
	memset(tree_root, 0, sizeof(vfs_node_t));
	strncpy((char *)&tree_root->name, "/", MAX_FILENAME);

	tree_root->v_type = VFS_DIR;
	tree_root->op.touch   = &ramfs_touch;
	tree_root->op.remove  = &ramfs_remove;
	tree_root->op.mkdir   = &ramfs_mkdir;
	tree_root->op.readdir = &ramfs_readdir;
}

vfs_node_ptr_t namei_path_to_vfs_node(vfs_node_ptr_t root, char *path, vfs_node_ptr_t *parent)
{
	size_t path_len, prev_idx;
	vfs_node_ptr_t tmp, prev;
	char fname[MAX_FILENAME];

	/* Non absolute path */
	if (path[0] != '/') {
		*parent = NULL;
		return NULL;
	}

	path_len = strlen(path)+1;
	prev_idx = _path_deconcatenator(path, path_len, 1, (char *)&fname);
	
	if (root == NULL)
		tmp = tree_root;
	else
		tmp = root;

	while (tmp != NULL) {
		prev = tmp;

		/* Return tree root */
		if (fname[0] == '\0') {
			*parent = tmp;
			break;
		}

		tmp = _find_child(tmp, fname);
		
		/* Child not found */
		if (tmp == NULL) {
			/* Last piece of path, so return parent */
			if (prev_idx == 0)
				*parent = prev;
			else
				*parent = NULL;
			
			break;
		}

		/* vfs_node was found */
		if (prev_idx == 0) {
			*parent = prev;
			break;
		}

		/* Get name of next child */
		prev_idx = _path_deconcatenator(path, path_len, prev_idx, (char *)&fname);
	}

	return tmp;
}

void namei_path_to_filename(const char *path, char *buff)
{
	size_t last_idx, i, len;

	last_idx = 0;
	i = 0;
	while (path[i] != '\0') {
		if (path[i] == '/' && path[i+1] != '\0') {
			last_idx = i;
		}
		i++;
	}

	strncpy(buff, &path[last_idx+1], MAX_FILENAME);
	
	/* Remove '/' from filename */
	len = strlen(buff);
	if (buff[len-1] == '/') {
		buff[len-1] = '\0';
	}
}

void namei_add_child(vfs_node_ptr_t parent, vfs_node_ptr_t child)
{
	child->parent = parent;
	
	if (parent->first_child == NULL) {
		child->prev_sibling = child->next_sibling = NULL;
		parent->first_child = parent->last_child = child;
		return;
	}

	parent->last_child->next_sibling = child;
	child->prev_sibling = parent->last_child;
	child->next_sibling = NULL;
	parent->last_child = child;
}

void namei_remove_child(vfs_node_ptr_t child)
{
	vfs_node_ptr_t parent;

	parent = child->parent;
	
	if (child->prev_sibling == NULL) {
		parent->first_child = child->next_sibling;
		if (parent->first_child != NULL) {
			parent->first_child->prev_sibling = NULL;
			return;
		}
		// No return here, bc when child is alone we need to set
		// parent->last_child too
	}

	if (child->next_sibling == NULL) {
		parent->last_child = child->prev_sibling;
		if (parent->last_child != NULL)
			parent->last_child->next_sibling = NULL;
		return;
	}

	child->prev_sibling->next_sibling = child->next_sibling;
	child->next_sibling->prev_sibling = child->prev_sibling;
}

int namei_mount(vfs_node_ptr_t mp, vfs_node_ptr_t fs_root)
{
	vfs_node_ptr_t tmp;

	/* Mount point is not empty! */
	if (mp->v_type != VFS_DIR && mp->first_child != NULL)
		return 1;

	mp->v_type = VFS_MNTPT;
	mp->op = fs_root->op;
	mp->first_child = fs_root->first_child;
	mp->last_child = fs_root->last_child;

	/* Change parent filed */
	tmp = mp->first_child;
	while (tmp != NULL) {
		tmp->parent = mp;
		tmp = tmp->next_sibling;
	}

	return 0;
}
