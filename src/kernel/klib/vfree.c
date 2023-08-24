#include <kernel/klib/_internal_vmalloc.h>
#include <kernel/klib/stdio.h>
#include <kernel/klib/stdlib.h>

#include <kernel/vmm.h>

extern struct used_regions ureg;

regptr_t _remove_region(uintptr_t vaddr)
{
	regptr_t region;

	region = ureg.first;
	while(region != NULL) {
		if (region->vaddr == vaddr) {
			/* Remove from the beginnig */
			if (region == ureg.first) {
				ureg.first = region->next;
				if (ureg.first != NULL)
					ureg.first->prev = NULL;
				else // Removing last element
					ureg.last = NULL;
			} else if (region == ureg.last) {
			/* Remove from the end of llist */
				ureg.last = region->prev;
				ureg.last->next = NULL;
			} else {
				region->prev->next = region->next;
				region->next->prev = region->prev;
			}

			ureg.used_pages -= region->size / PAGE_SIZE;
			break;
		}

		region = region->next;
	}

	return region;
}

void vfree(void *ptr)
{
	uintptr_t vaddr;
	regptr_t region;

	vaddr = (uintptr_t)ptr;
	region = _remove_region(vaddr);
	if (region == NULL)
		panic("[ERROR] vfree tried to release unused memory");

	if (region->phy_type == PHY_CONT) {
		vmm_cont_unmap(region->vaddr, region->size, 1);
	} else {
		vmm_cont_unmap(region->vaddr, region->size, 0);
	}

	kfree(region);
}
