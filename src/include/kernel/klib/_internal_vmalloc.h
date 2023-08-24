#ifndef __internal_vmalloc_dot_H
#define __internal_vmalloc_dot_H

#include <stdint.h>
#include <stddef.h>
#include <kernel/klib/stdlib.h>

struct region_hdr {
	uintptr_t vaddr;
	size_t size;
	vmalloc_req_t phy_type;

	/* Linked list of used regions */
	struct region_hdr *prev;
	struct region_hdr *next;
};
typedef struct region_hdr* regptr_t;

struct used_regions {
	regptr_t first;
	regptr_t last;
	size_t used_pages;
};

#endif

