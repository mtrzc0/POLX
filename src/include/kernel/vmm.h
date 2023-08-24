#ifndef __vmm_dot_H
#define __vmm_dot_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <kernel/vfs.h>
#include <kernel/arch.h>

/* Privilege and access flags */
//#define VMM_SHARED	0x1		/* DO NOT use with paging misc */
#define VMM_RW		(0x1 << 1)	/* Compatible with paging misc */
#define VMM_USER 	(0x1 << 2)	/* Compatible with paging misc */
#define VMM_FORK	(0x1 << 3)	/* DO NOT use with paging misc */

struct vmm_region {
	uintptr_t addr;
	size_t size;
	size_t fork_ctr;
	uint32_t flags;

	/* Linked list of regions */
	struct vmm_region *prev, *next;
};
typedef struct vmm_region region_t;

struct vmm_aspace {
	uintptr_t pd;
	uintptr_t code_entry;
	uintptr_t data_end;	/* End of data segment */
	uintptr_t stack_end;

	size_t stack_size;
	size_t page_counter;
	region_t *first;
	region_t *last;
};
typedef struct vmm_aspace vmm_aspace_t;

/* Wrappers for arch paging */
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

/* Paging misc functions */
void vmm_cont_map(uintptr_t vaddr, size_t bytes, 
		  uint32_t flags, bool phy_cont);

void vmm_void_map(uintptr_t vaddr, size_t bytes, uint32_t flags);

void vmm_cont_unmap(uintptr_t vaddr, size_t bytes, bool phy_cont);
void vmm_void_unmap(uintptr_t vaddr, size_t bytes);

/* Virtual address space management */
vmm_aspace_t *vmm_aspace_create(vmm_aspace_t *parent_as);
void vmm_aspace_destroy(vmm_aspace_t *aspace);
int vmm_mmap_at(vmm_aspace_t *as, uintptr_t vaddr, vfs_node_ptr_t obj,
		size_t offset, size_t size, size_t align, uint32_t flags);
int vmm_munmap(vmm_aspace_t *as, uintptr_t vaddr);
char *vmm_copy_string_from(vmm_aspace_t *as, uintptr_t vaddr, size_t max_len);
int vmm_copy_data_to(vmm_aspace_t *as, uintptr_t vaddr, void *buff, size_t size);
void vmm_set_stack(vmm_aspace_t *as, uintptr_t stack_end, size_t size);
void vmm_page_fault_handler(vmm_aspace_t *as, uintptr_t vaddr);
#endif
