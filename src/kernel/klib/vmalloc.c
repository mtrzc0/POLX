#include <kernel/klib/_internal_vmalloc.h>
#include <kernel/klib/stdio.h>
#include <kernel/klib/stdlib.h>

#include <kernel/arch.h>
#include <kernel/vmm.h>

#include <stdbool.h>

struct used_regions ureg = { 0 };

static uintptr_t _find_hole_in_as(size_t size, regptr_t *prev)
{
	regptr_t tmp, next;

	/* Found at the beginning of the address space */
	if (ureg.first->vaddr > VM_VMALLOC_START &&
	   (ureg.first->vaddr - VM_VMALLOC_START) >= size)
	{
		*prev = NULL;
		return VM_VMALLOC_START;
	}

	/* Search for holes between used regions */
	tmp = ureg.first;
	next = tmp->next;
	while (next != NULL) {
		if ((next->vaddr - tmp->vaddr - tmp->size) >= size) {
			*prev = tmp;
			return (tmp->vaddr + tmp->size);
		}
		tmp = tmp->next;
		next = tmp->next;
	}
	
	/* Found at the end of the address space */
	if ((ureg.last->vaddr + ureg.last->size) < VM_VMALLOC_END &&
	   (VM_VMALLOC_END - ureg.last->vaddr - ureg.last->size) >= size)
	{
		*prev = ureg.last;
		return (ureg.last->vaddr + ureg.last->size);
	}

	/* Not found :( */
	return 0;
}

static void _insert_region(regptr_t prev, regptr_t new)
{
	/* Init used region struct on first run */
	if (ureg.first == NULL) {
		ureg.first = new;
		ureg.last = new;
		ureg.used_pages = new->size / PAGE_SIZE;
		new->prev = NULL;
		new->next = NULL;
		return;
	}

	if (prev == NULL) {
	/* Insert on start of llist */
		new->prev = NULL;
		new->next = ureg.first;
		ureg.first->prev = new;
		ureg.first = new;
	} else if (ureg.last == prev) {
	/* Insert on end of llist */
		new->next = NULL;
		new->prev = prev;
		prev->next = new;
		ureg.last = new;
	} else {
	/* Insert between two nodes */
		new->prev = prev;
		new->next = prev->next;
		new->next->prev = new;
		prev->next = new;
	}
		
	ureg.used_pages += new->size / PAGE_SIZE;
}

void *vmalloc(size_t size, vmalloc_req_t phy_type)
{
	uintptr_t vaddr;
	regptr_t prev_region, new_region;
	
	prev_region = NULL;
	new_region = NULL;

	/* Align size to page boundary */
	if (size % PAGE_SIZE > 0)
		size = (size - size % PAGE_SIZE) + PAGE_SIZE;

	/* Find free hole in memory */
	if (ureg.first == NULL) {
		vaddr = VM_VMALLOC_START;
	} else {
		vaddr = _find_hole_in_as(size, &prev_region);
		if (vaddr == 0)
			panic("[PANIC] vmalloc cannot satisfy the request");
	}

	/* Map physical memory based on phy_type */
	switch (phy_type) {
	case MAPPED:
		vmm_cont_map(vaddr, size, VMM_RW, 0);
		break;
	case PHY_CONT:
		vmm_cont_map(vaddr, size, VMM_RW, 1);
		break;
	case ON_DEMAND:
		vmm_void_map(vaddr, size, VMM_RW);
		break;
	default:
		panic("[ERROR] vmalloc unknown phy_type!");
	};

	new_region = (regptr_t) kmalloc(sizeof(struct region_hdr));
	new_region->vaddr = vaddr;
	new_region->size = size;
	new_region->phy_type = phy_type;

	_insert_region(prev_region, new_region);

	return (void *)vaddr;
}
