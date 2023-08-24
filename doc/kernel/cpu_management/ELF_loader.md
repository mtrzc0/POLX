# _<kernel/elf_loader.h>_

Table of contents:    

0. [API](#api)   
    - [Load elf file](#load-elf-file)   

## API
### Load elf file
```c
int elf_load(vmm_aspace_t *as, vfs_node_ptr_t file);
```
Load elf from _file_ into _as_   