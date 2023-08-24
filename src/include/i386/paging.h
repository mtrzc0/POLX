#ifndef __paging_dot_H

#include <stdint.h>

/* PD & PT entry
	0		11 12		   31
	+-----------------+-----------------+
	|     Flags       |     Address     |
	+-----------------+-----------------+

Flags details:
	Intel® 64 and IA-32 Architectures Developer's Manual: Vol. 3A 
	Tables 4-5 and 4-6 (page 116).
*/

#define PG_PRESENT	0x1
#define PG_RW		(0x1 << 1)
#define PG_USER		(0x1 << 2)
#define PG_ALIGN	0xfffff000

#define PG_KERN_PAGE_FLAG PG_PRESENT | PG_RW
#define PG_KERN_PAGE_RO_FLAG PG_PRESENT
#define PG_USER_PAGE_FLAG PG_PRESENT | PG_RW | PG_USER
#define PG_USER_PAGE_RO_FLAG PG_PRESENT | PG_USER

void pg_switch_pd(uint32_t new);

void pg_map(uint32_t paddr, uint32_t vaddr, uint32_t flags);
void pg_map_to(uint32_t pd, uint32_t paddr, uint32_t vaddr, uint32_t flags);

/* Return physical address to copy of vaddr page */
uint32_t pg_duplicate_page(uint32_t pd, uint32_t vaddr);

/* Return physical address of frame that was mapped */
uint32_t pg_unmap(uint32_t vaddr);
uint32_t pg_unmap_from(uint32_t pd, uint32_t vaddr);
uint32_t pg_get_frame_from(uint32_t pd, uint32_t vaddr);

uint32_t pg_create_directory(uint32_t parent_pd);
/* Return new recursive mapped dir with copied kernel part */
void pg_destroy_directory(uint32_t pd);

void pg_page_fault_handler(uint32_t err_code);

#endif
