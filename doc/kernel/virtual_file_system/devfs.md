# ***Treat device as file***
Table of contents:   
0. [API](#api)   
    - [Init](#initialization)   
    - [Add device](#add-device)   
    - [Remove device](#remove-device)   
1. [Pseudo devices](#pseudo-devices)   
    - [/dev/zero](#zero)   
    - [/dev/stdout](#stdout)   

# _<kernel/devfs.h>_

## API
### Initialization
```c
void devfs_init(void)
```
Init devfs directory and mount pseudo devices

### Add device
```c
int devfs_add_dev(vfs_node_ptr_t dev);
```
Add device node to vfs_tree

### Remove device
```c
vfs_node_ptr_t devfs_remove_dev(char *name);
```
Remove device node from vfs_tree and return device vnode

## Pseudo devices
Available to the kernel via global pointers   
### zero
```c
extern vfs_node_ptr_t dev_zero;
```
"/dev/zero" same feautures as unix counterpart  

### stdout
```c
extern vfs_node_ptr_t dev_stdout;
```
"/dev/stdout" same features as unix counterpart