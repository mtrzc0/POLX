# ***"Managing address spaces"***
This module is responsible for creating/destroing address spaces for process and to handle memory mapping with-in.  Module is also used as wrapper to arch dependent paging functions. 

# TODO
1. Handling child process to write into forked data (vmm_page_fault_handler)   

# _<kernel/vmm.h>_
Table of contents:   
0. [Used data structures](#data-structures)   
1. [Paging wrappers](#paging-wrappers-and-misc)    

2. [API](#api)
    - [Create address space](#create-address-space)   
    - [Destroy address space](#destroy-address-space)   
    - [Map object](#map-object-at)   
    - [Unmap object](#unmap-object)   
    - [Copy string from](#copy-string-from-target-as-to-kernel-as)   
    - [Copy data to](#copy-data-from-kernel-as-to-target-as)   
    - [Set stack](#set-stack)   
    - [Page faut check](#user-page-fault-handler)   

## Data structures

### Memory region
```c
/* Privilege and access flags */
//#define VMM_SHARED      0x1
#define VMM_RW		(0x1 << 1)	/* Compatible with paging misc */
#define VMM_USER 	(0x1 << 2)	/* Compatible with paging misc */
#define VMM_FORK	(0x1 << 3)	/* DO NOT use with paging misc */

struct vmm_region {
	void *addr;
	size_t size;
    size_t fork_ctr;
	uint32_t flags;
    
    /* Linked list of regions */
	struct vmm_region *prev, *next;
};
typedef struct vmm_region region_t;
```

### Task VM
```c
struct vmm_aspace {
    uintptr_t pd;
    uintptr_t code_entry;
    uintptr_t data_end;     /* End of data segment */
    
	size_t page_counter;
	region_t first;
	region_t last;
};
typedef struct vmm_aspace aspace_t;
```

## Paging wrappers and misc
### Wrappers
```c
#define vmm_map(paddr, vaddr, flags) \
	pg_map((uint32_t)(paddr), (uint32_t)(vaddr), (uint32_t)(flags))

#define vmm_map_to(pd, paddr, vaddr, flags) \
	pg_map_to((uint32_t)(pd), (uint32_t)(paddr), \
		     (uint32_t)(vaddr), (uint32_t)(flags))

#define vmm_duplicate_page(pd, vaddr) \
    pg_duplicate_page((uint32_t)(pd), (uint32_t)(vaddr))

#define vmm_unmap(vaddr) \
	pg_unmap((uint32_t)(vaddr))

#define vmm_unmap_from(pd, vaddr) \
	pg_unmap_from((uint32_t)(pd), (uint32_t)(vaddr))

#define vmm_get_frame_from(pd, vaddr) \
	pg_get_frame_from((uint32_t)(pd), (uint32_t)(vaddr))

#define vmm_create_directory(parent_pd) \
	pg_create_directory((uint32_t)(parent_pd))

#define vmm_destroy_directory(paddr) \
	pg_destroy_directory((uint32_t)paddr)
```
### Linear mapping
```c
/* Every entry has already allocated physical frame */
/* No page fault on access */
void vmm_cont_map(uintptr_t vaddr, /* Beginning address */
                  size_t bytes,    /* Size of map */
                  uint32_t flags,  /* PTE flags */
                  bool phy_cont);  /* Should be physicaly contiguous */

/* Only map region as reserved, but don't allocate any physical memory */
/* Page fault on access */
void vmm_void_map(uintptr_t vaddr, size_t bytes, uint32_t flags);
```

### Linear unmapping
```c
void vmm_cont_unmap(uintptr_t vaddr, /* Beginnig address */
                    size_t bytes,    /* Allocated size */
                    bool phy_cont);  /* Is physicaly contiguous */

void vmm_void_unmap(uintptr_t vaddr, size_t bytes);
```

##  API

### Create address space
```c
vmm_aspace_t *vmm_aspace_create(vmm_aspace_t *parent_as);
```
Allocate and initialize task aspace struct, if _parent_as_ not NULL fork user space to new address space.

### Destroy address space
```c
void vmm_aspace_destroy(vmm_aspace_t *aspace);
```
Free all objects in aspace and destroy task aspace struct.

### Map object at
```c
int vmm_mmap_at(vmm_aspace_t *as, uintptr_t vaddr, vfs_node_ptr_t obj,
                size_t offset, size_t size, size_t align, uint32_t flags);
```
Copy _size_ data from _obj_ + _offset_ to task address space at _vaddr_

### Unmap object
```c
int vmm_munmap(vmm_aspace_t *as, uintptr_t vaddr);
```
Zero region if another regions are on the same page, or unmap page

### Copy string from target AS to kernel AS
```c
char *vmm_copy_string_from(vmm_aspace_t *as, uintptr_t vaddr, size_t max_len);
```
Copy string from target AS into kmalloc buffer and return address of it.
**WARNING: You must take care of releasing (kfree) this buffer when it is no longer needed**

### Copy data from kernel AS to target AS
```c
int vmm_copy_data_to(vmm_aspace_t *as, uintptr_t vaddr, void *buff, size_t size);
```
Copy _size_ data from _buff_ to _as_ at _vaddr_

### Set stack
```c
void vmm_set_stack(vmm_aspace_t *as, uintptr_t stack_end, size_t size);
```
Fill proper fileds in _as_ with given values

### User page fault handler
```c
void vmm_page_fault_handler(vmm_aspace_t *as, uintptr_t vaddr);
```