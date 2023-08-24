# ***RAM as volatile hard drive***
Idea is simple ramfs is a driver used to satisfy VFS demands. Everything exist in RAM memory and it is not mapped in VM, unless mmap syscall is trigered. First version support only files that fit in one physical frame. Directories exist only as vfs_nodes and they are
not using physical frames.

# _<kernel/ramfs.h>_

## INODE
```c
struct ramfs_inode {
	uintptr_t paddr; /* Address of frame holding data */
};
typedef ramfs_inode_t;
```
## ramfs_open
```c
int ramfs_open(vfs_node_ptr_t node, int mode);
```
Always return true

## ramfs_close
```c
int ramfs_close(vfs_node_ptr_t node);
```
Always return true

## ramfs_read
```c
long ramfs_read(vfs_node_ptr_t node, size_t offset, size_t size, char *buffer);
```
Copy _size_ data from node->data->paddr + _offset_ to _buffer_ and return amount of copied data

## ramfs_write
```c
long ramfs_write(vfs_node_ptr_t node, size_t offset, char *buffer, size_t size);
```
Copy _size_ data from _buffer_ to node->data->paddr+_offset_ and return amount of copied data

## ramfs_touch
```c
int ramfs_touch(vfs_node_ptr_t p_node, vfs_node_ptr_t *node, char *name, int mode);
```
- Reserve space for ramfs_inode_t, vfs_node_t
- Ask pmm for frame 
- Fill ramfs_inode_t and vfs_node_t
- return true if success

## ramfs_remove
```c
int ramfs_remove(vfs_node_ptr_t node);
```
- Free frame 
- Free ramfs_inode_t
- return true

## ramfs_mkdir
```c
int ramfs_mkdir(vfs_node_ptr_t p_node, vfs_node_ptr_t *node, char *name, int mode);
```
reserve space for _node_ and fill it

## ramfs_rmdir
```c
int ramfs_rmdir(vfs_node_ptr_t node);
```
return true

## ramfs_readdir
```c
int ramfs_readdir(vfs_node_ptr_t node, size_t no, vfs_dirent_t *dent);
```
write name of _no_ child  _node_ into _dent_ struct