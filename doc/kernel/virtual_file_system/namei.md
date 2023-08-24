# ***Path to vfs_node vfs tree***
Table of contents:   
0. [API](#api)   
    - [Init](#initialization)   
    - [Get vfs_node](#get-vfs_node)   
    - [Get filename](#get-filename)   
    - [Add child](#add-child)   
    - [Remove child](#remove-child)   
    - [Mount](#mount)   

# _<kernel/namei.h>_

## API
### Initialization
```c
void namei_init(void);
```
Create root of vfs tree.

### Get vfs_node
```c
vfs_node_ptr_t namei_path_to_vfs_node(vfs_node_ptr_t root, char *path, vfs_node_ptr_t *parent);
```
Go down the  _root_ or vfs_tree if _root_ is NULL with the given path, until found searched node or path is not correct. Use only with absolute paths.   

**Return:**   
- ***Last node does not exist*** - return NULL and set _parent_ to penultimate node given in _path_   
- ***Wrong path*** - return NULL ans set _parent_ to NULL   
- ***Path resolved succesfuly*** - return node and set _parent_ to node parent   
 
### Get filename
 ```c
void namei_path_to_filename(char *path, char *buff);
```
Extract filename from path adn save it to buff   
 
### Add child
```c
void namei_add_child(vfs_node_ptr_t parent, vfs_node_ptr_t child);
```
Add child to given parent node.

### Remove child
```c
void namei_remove_child(vfs_node_ptr_t child);
```
Remove child from vfs tree.

### Mount
```c
int namei_mount(vfs_node_ptr_t mp, vfs_node_ptr_t fs_root);
```
Mount _fs_root_ at _mp_ in vfs tree only when _mp_ dir is empty.