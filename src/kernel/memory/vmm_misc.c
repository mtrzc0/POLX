#include <kernel/vmm.h>
#include <kernel/pmm.h>

#define _SIZE_TO_FRAMES(s) \
	((s)%FRAME_SIZE == 0) ? (s)/FRAME_SIZE : ((s)/FRAME_SIZE)+1

void vmm_cont_map(uintptr_t vaddr, /* Start vaddr for mapping */
		  size_t bytes,	   /* How many bytes to map */
		  uint32_t flags,  /* PTE flags */
		  bool phy_cont )  /* Map with phy contiguous mem */
{
	uintptr_t paddr;
	size_t frames;

	frames = _SIZE_TO_FRAMES(bytes);

	if (phy_cont) {
		paddr = pmm_get_pool(frames);
		for (size_t i=0; i < frames; i++) {
			vmm_map(paddr, vaddr, flags | PG_PRESENT);
			paddr += FRAME_SIZE;
			vaddr += FRAME_SIZE;
		}
	} else {
		for (size_t i=0; i < frames; i++) {
			vmm_map(pmm_get_frame(), vaddr, flags | PG_PRESENT);
			vaddr += FRAME_SIZE;
		}
	}
}

void vmm_void_map(uintptr_t vaddr, size_t bytes, uint32_t flags)
{
	size_t frames;

	frames = _SIZE_TO_FRAMES(bytes);

	for (size_t i=0; i < frames; i++) {
		vmm_map(0, vaddr, flags);
		vaddr += FRAME_SIZE;
	}
}

void vmm_cont_unmap(uintptr_t vaddr, size_t bytes, bool phy_cont)
{
	uintptr_t paddr;
	size_t frames;

	frames = _SIZE_TO_FRAMES(bytes);

	if (phy_cont) {
		paddr = vmm_unmap(vaddr);
		vaddr += FRAME_SIZE;
		for (size_t i=1; i < frames; i++) {
			vmm_unmap(vaddr);
			vaddr += FRAME_SIZE;
		}
		pmm_free_pool(paddr, frames);
	} else {
		for (size_t i=0; i < frames; i++) {
			paddr = vmm_unmap(vaddr);
			if (paddr > 0)
				pmm_free_frame(paddr);
			vaddr += FRAME_SIZE;
		}
	}
}

void vmm_void_unmap(uintptr_t vaddr, size_t bytes)
{
	uintptr_t paddr;
	size_t frames;

	frames = _SIZE_TO_FRAMES(bytes);

	for (size_t i=0; i < frames; i++) {
		paddr = vmm_unmap(vaddr);
		
		/* Page was mapped */
		if (paddr)
			pmm_free_frame(paddr);
	}
}
