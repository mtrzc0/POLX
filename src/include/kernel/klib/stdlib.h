#ifndef __kernel_stdlib_dot_H
#define __kernel_stdlib_dot_H
#include <stddef.h>

void *kmalloc(size_t size);
void kfree(void *ptr);

typedef enum {
	MAPPED,		/* Mapped in virtual memory, no page fault on access */
	PHY_CONT,	/* Mapped with physical contiguous memory */
	ON_DEMAND 	/* Marked as used, page fault on first access */
} vmalloc_req_t;

void *vmalloc(size_t size, vmalloc_req_t phy_type);
void vfree(void *ptr);

#endif
