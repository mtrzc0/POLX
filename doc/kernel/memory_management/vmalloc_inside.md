# *"Higher half memory manager"*
Purpose of vmalloc and vfree is to manage memory space specified in _<kernel/arch.h>_. It allocate chunks with a size that is a multiple of the page size and aligned to page boundary. That allow it to allocate memory for DMA or paging structures.

# _<kernel/klib/\_internal\_vmalloc.h>_
Table of contents:   
0. [Used data structures](#data-structures)   
1. [Allocate memory](#vmalloc)   
2. [Release memory](#vfree)   

## Data structures

### request type
```c
<kernel/klib/stdlib.h>
typedef enum {
	MAPPED,		/* Mapped in virtual memory, no page fault on access */
	PHY_CONT,	/* Mapped with physical contiguous memory */
	ON_DEMAND 	/* Marked as used, page fault on first access */
} vmalloc_req_t;
```

### region header
```c
struct region_hdr {
	uintptr_t vaddr;
	size_t size;
	vmalloc_req_t phy_type;

	/* Linked list of used regions */
	struct region_hdr *prev;
	struct region_hdr *next;
};
typedef struct region_hdr* regptr_t;
```
### used regions
```c
struct used_regions {
	regptr_t first;
	regptr_t last;
	size_t used_pages;
};
``` 

# vmalloc
## Algorithm
- Find the right place in memory   
- If not found   
    - panic   
  
- Map memory according to memory type field   
- Allocate and fill region header struct 
- Insert region into used region llist   
- return vaddr

# vfree
- Remove region from used region llist
- Unmap memory according to memory type filed
- Release region header struct