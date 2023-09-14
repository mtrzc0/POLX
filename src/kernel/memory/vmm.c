#include <errno.h>
#include <stddef.h>

#include <kernel/vmm.h>
#include <kernel/pmm.h>
#include <kernel/arch.h>
#include <kernel/task.h>
#include <kernel/klib.h>
#include <kernel/signal.h>

extern int errno;
extern task_t *actual_task;

static int _is_area_free(vmm_aspace_t *as, uintptr_t addr, size_t size)
{
	/* Check if area [addr, (addr+size)] is not used */
	region_t *tmp;

	/* Whole AS is free */
	if (as->first == NULL)
		return 1;

	/* Check end of allocated regions */
	if (addr >= (as->last->addr + as->last->size)) {
		if ((addr+size) <= VM_USER_END)
			return 1;
		else
			return 0;
	}

	tmp = as->first;
	while (tmp != NULL) {
		if (addr <= tmp->addr) {
			if ((addr+size) <= tmp->addr)
				return 1;
			else
				return 0;
		}
		
		// if (addr > tmp->addr)
		tmp = tmp->next;
	}

	return 0;
}

static void _insert_region(vmm_aspace_t *as, region_t *reg)
{
	region_t *tmp;

	/* Insert at the beginning */
	if (as->first == NULL || as->first->addr >= (reg->addr+reg->size)) {
		reg->next = as->first;
		as->first = reg;
		
		if (reg->next != NULL)
			reg->next->prev = reg;

		reg->prev = NULL;
	
		/* Initialize llist head */
		if (as->last == NULL)
			as->last = reg;
		
		return;
	}

	/* Insert at the end*/
	if ((as->last->addr+as->last->size) <= reg->addr) {
		reg->next = NULL;
		reg->prev = as->last;
		as->last = reg;
		reg->prev->next = reg;
		return;
	}

	/* Insert between regions */
	tmp = as->first;
	while (tmp->next != NULL) {
		if (reg->addr >= (tmp->addr+tmp->size) &&
		   (reg->addr+reg->size) <= tmp->next->addr) {
			reg->next = tmp->next;
			tmp->next->prev = reg;
			reg->prev = tmp;
			tmp->next = reg;	
			break;
		}

		tmp = tmp->next;
	}
}

static void _remove_region(vmm_aspace_t *as, region_t *reg)
{
	int done;

	done = 0;
	if (as->first->addr == reg->addr) {
		as->first = reg->next;
		if (reg->next != NULL)
			reg->next->prev = NULL;
		done = 1;
	}

	if (as->last->addr == reg->addr) {
		as->last = reg->prev;
		if (reg->prev != NULL)
			reg->prev->next = NULL;
		
		return;
	} else if (done) {
		return;
	}

	reg->prev->next = reg->next;
	reg->next->prev = reg->prev;
}

static region_t *_find_region(vmm_aspace_t *as, uintptr_t vaddr)
{
	region_t *tmp;

	tmp = as->first;

	while (tmp != NULL) {
		if (vaddr >= tmp->addr && vaddr <= (tmp->addr+tmp->size))
			break;
		tmp = tmp->next;
	}

	return tmp;
}

static region_t *_create_region(uintptr_t addr, size_t size, uint32_t flags)
{
	region_t *reg;
	
	reg = kmalloc(sizeof(region_t));
	memset(reg, 0, sizeof(region_t));

	reg->addr = addr;
	reg->size = size;
	reg->flags = flags;

	return reg;
}

static void _destroy_region(vmm_aspace_t *as, region_t *reg)
{
	uintptr_t paddr, tmp;
	_remove_region(as, reg);
	
	/* Free physical memory */
	tmp = reg->addr;
	tmp = (tmp % PAGE_SIZE) ? tmp : tmp - (tmp % PAGE_SIZE);
	while (tmp < reg->addr + reg->size) {
		paddr = vmm_unmap_from(as->pd, tmp);
		if (paddr)
			pmm_free_frame(paddr);
		tmp += PAGE_SIZE;
	}
	
	kfree(reg);
}

static uintptr_t _find_free_area(vmm_aspace_t *as, size_t size)
{
	uintptr_t vaddr;
	region_t *reg;

	/* Check address space from stack_end - 0x1000 to data_end + 0x1000 */
	vaddr = as->stack_end - 0x1000 - size;
	while (vaddr >= as->data_end + 0x1000) {
		if (_is_area_free(as, vaddr, size)) {
			return vaddr;
		} else {
		/* 
		  It is more efficient to use found regions addr while
		  moving back then doing it by subtracting future area size
		  but if _find_region will not find region that mean
		  there is a free space between regions and we have to,
		  use subtraction technique to check if that space is
		  sufficient 
		*/
			reg = _find_region(as, vaddr);
			if (reg == NULL) {
				vaddr -= size;
			} else {
				vaddr = reg->addr - size;
			}
		}
	}

	/* No free area found */
	return (uintptr_t)NULL;
}

static uintptr_t _vaddr_to_paddr(uintptr_t pd, uintptr_t vaddr)
{
	uintptr_t paddr;

	paddr = vmm_get_frame_from(pd, vaddr);

	return paddr;
}

static void _fork_aspace(vmm_aspace_t *src, vmm_aspace_t *dst)
{
	uintptr_t vaddr, paddr;
	region_t *tmp, *new;

	tmp = src->first;
	while (tmp != NULL) {
		vaddr = tmp->addr;
		while (vaddr < (tmp->addr + tmp->size)) {
			paddr = vmm_duplicate_page(src->pd, vaddr);
			vmm_map_to(dst->pd, paddr, vaddr, tmp->flags | PG_PRESENT);
			vaddr += PAGE_SIZE;
		}

		new = _create_region(tmp->addr, tmp->size, tmp->flags);
		_insert_region(dst, new);

		tmp = tmp->next;
	}

	dst->code_entry = src->code_entry;
	dst->elf_end = src->elf_end;
	dst->data_end = src->data_end;
	dst->stack_end = src->stack_end;
	dst->stack_size = src->stack_size;
	dst->page_counter = src->page_counter;
}

vmm_aspace_t *vmm_aspace_create(vmm_aspace_t *parent_as)
{
	vmm_aspace_t *new;

	new = kmalloc(sizeof(vmm_aspace_t));
	memset(new, 0, sizeof(vmm_aspace_t));

	if (parent_as == NULL) {
		new->pd = vmm_create_directory(NULL);
	} else {
		new->pd = vmm_create_directory(parent_as->pd);
		_fork_aspace(parent_as, new);
	}

	return new;
}

void vmm_aspace_destroy(vmm_aspace_t *aspace)
{
	region_t *region, *tmp;

	/* Unmap regions */
	region = aspace->first;
	
	while (region != NULL) {
		tmp = region->next;
		_destroy_region(aspace, region);
		region = tmp;
	}

	kfree(aspace);
}

void *vmm_mmap_at(vmm_aspace_t *as, uintptr_t vaddr, vfs_node_ptr_t obj, 
		size_t offset, size_t size, size_t align, uint32_t flags)
{
	region_t *region;
	uintptr_t paddr, tmp, current_vaddr;
	size_t rem_to_cpy, cpy_int, current_offset, page_ctr, aligned_size;
	char *buffer;

	/* Align region */
	if (align == 0) {
		align = 0x1000;
	}
	
	aligned_size = size;
	if (size % align != 0) {
		aligned_size = (aligned_size - aligned_size % align) + align;
		aligned_size = aligned_size - (vaddr % align);
	}

	/* Find free space for allocation */
	if (vaddr == (uintptr_t)NULL) {
		vaddr = _find_free_area(as, aligned_size);
		if (vaddr == (uintptr_t)NULL) {
			errno = ENOMEM;
			return NULL;
		}
	}

	if (! _is_area_free(as, vaddr, aligned_size)) {
		errno = ENOMEM;
		return NULL;
	}

	if (size == 0) {
		errno = 0;
		return NULL;
	}

	tmp = vmm_unmap(VM_TMP2_MAP);

	/* Copy data to one or more phisical pages */
	page_ctr = 0;
	rem_to_cpy = size;
	current_vaddr = vaddr;
	current_offset = offset;
	while (rem_to_cpy > 0) {
		paddr = _vaddr_to_paddr(as->pd, current_vaddr);
		if (! paddr) {
			paddr = pmm_get_frame();
			page_ctr++;
		}

		vmm_map(paddr, VM_TMP2_MAP, PG_KERN_PAGE_FLAG);

		/* Out of page boundary */
		if ((current_vaddr % PAGE_SIZE) + rem_to_cpy > PAGE_SIZE) {
			cpy_int = PAGE_SIZE - (current_vaddr % PAGE_SIZE);
		} else {
		/* Copy rest data */
			cpy_int = rem_to_cpy;
		}
		
		buffer = (char *)(VM_TMP2_MAP + (current_vaddr % PAGE_SIZE));

		if (vfs_read(obj, current_offset, cpy_int, buffer) < 0) {
			vmm_unmap(VM_TMP2_MAP);
			vmm_map(tmp, VM_TMP2_MAP, PG_KERN_PAGE_FLAG);
			return NULL;
		}
		
		/* Save changes to dst PD */
		vmm_map_to(as->pd, paddr, current_vaddr, flags | PG_PRESENT);

		rem_to_cpy -= cpy_int;
		current_vaddr += cpy_int;
		current_offset += cpy_int;
		
		vmm_unmap(VM_TMP2_MAP);
	}

	region = _create_region(vaddr, aligned_size, flags);
	_insert_region(as, region);
	as->page_counter += page_ctr;

	/* Restore previous value of VM_TMP2_MAP */
	vmm_map(tmp, VM_TMP2_MAP, PG_KERN_PAGE_FLAG);
	return (void *)region->addr;
}

int vmm_munmap(vmm_aspace_t *as, uintptr_t vaddr)
{
	region_t *reg;
	uintptr_t paddr, tmp, current_vaddr;
	size_t rem_size, destroy_int, page_ctr;

	reg = _find_region(as, vaddr);
	if (reg == NULL) {
		errno = EINVAL;
		return -1;
	}

	tmp = vmm_unmap(VM_TMP2_MAP);

	page_ctr = 0;
	rem_size = reg->size;
	current_vaddr = reg->addr;	
	while (rem_size > 0) {
		paddr = _vaddr_to_paddr(as->pd, vaddr);
		if (! paddr)
			panic("[ERROR] VMM bad physical address of region!");
		
		vmm_map(paddr, VM_TMP2_MAP, PG_KERN_PAGE_FLAG);

		/* Calculate size to page boundary */
		destroy_int = PAGE_SIZE - (current_vaddr % PAGE_SIZE);
		if (destroy_int > rem_size)
			destroy_int = rem_size;
	
		/* Check if other regions are mapped on this page */
		if (destroy_int % PAGE_SIZE != 0) {
			/* Destroy content without unmapping page from original PD */
			if (reg->prev != NULL && (reg->addr - (reg->prev->addr+reg->prev->size) < PAGE_SIZE)) {
				memset((char *)(VM_TMP2_MAP+(current_vaddr % PAGE_SIZE)), 0, rem_size);
				rem_size -= destroy_int;
				current_vaddr += destroy_int;
				continue;
			}

			if (reg->next != NULL && (reg->next->addr - (reg->addr+reg->size)) < PAGE_SIZE) {
				memset((char *)(VM_TMP2_MAP+(current_vaddr % PAGE_SIZE)), 0, rem_size);
				rem_size -= destroy_int;
				current_vaddr += destroy_int;
				continue;
			}
		}

		/* Save changes */
		vmm_unmap_from(as->pd, current_vaddr);
		pmm_free_frame(paddr);

		rem_size -= destroy_int;
		current_vaddr += destroy_int;
		vmm_unmap(VM_TMP2_MAP);
		page_ctr++;
	}
	
	_destroy_region(as, reg);
	as->page_counter -= page_ctr;
	
	vmm_map(tmp, VM_TMP2_MAP, PG_KERN_PAGE_FLAG);
	return 0;
}

char *vmm_copy_string_from(vmm_aspace_t *as, uintptr_t vaddr, size_t max_len)
{
	char *buff, *src_buff;
	size_t i, i_after_next_page;
	uintptr_t paddr, old_tmp;

	paddr = vmm_get_frame_from(as->pd, vaddr);
	if (paddr) {
		old_tmp = vmm_unmap(VM_TMP_MAP);
		vmm_map(paddr, VM_TMP_MAP, PG_KERN_PAGE_FLAG);
	} else {
		errno = EFAULT;
		return NULL;
	}

	buff = (char *)kmalloc(max_len);
	src_buff = (char *)(VM_TMP_MAP + (vaddr % PAGE_SIZE));
	i = 0;
	i_after_next_page = 0;
	while (i < max_len) {
		/* Rest of the string is on the next page */
		if ((uintptr_t)&src_buff[i - i_after_next_page] % PAGE_SIZE == 0) {
			vaddr += i;
			vmm_unmap(VM_TMP_MAP);
			paddr = vmm_get_frame_from(as->pd, vaddr);
			if (!paddr) {
				kfree(buff);
				vmm_map(old_tmp, VM_TMP_MAP, PG_KERN_PAGE_FLAG);
				errno = EFAULT;
				return NULL;
			}
			vmm_map(paddr, VM_TMP_MAP, PG_KERN_PAGE_FLAG);
			src_buff = (char *)VM_TMP_MAP;
			
			/* 
			  Because after mapping next page we have to start
			  from the beginning of it
			*/
			i_after_next_page = i;
		}

		buff[i] = src_buff[i - i_after_next_page];
		if (src_buff[i - i_after_next_page] == '\0') {
			break;
		}

		i++;
	}

	vmm_unmap(VM_TMP_MAP);
	vmm_map(old_tmp, VM_TMP_MAP, PG_KERN_PAGE_FLAG);
	
	/* String is too long */
	if (i == max_len) {
		kfree(buff);
		errno = E2BIG;
		return NULL;
	}
	
	return buff;
}

int vmm_copy_data_to(vmm_aspace_t *as, uintptr_t vaddr, void *buff, size_t size)
{
	char *src, *dst;
	uintptr_t old_tmp, paddr;
	size_t i, i_after_next_page;

	paddr = _vaddr_to_paddr(as->pd, vaddr);
	if (! paddr) {
		errno = EFAULT;
		return -1;
	}

	i = 0;
	i_after_next_page = 0;
	old_tmp = vmm_unmap(VM_TMP_MAP);
	vmm_map(paddr, VM_TMP_MAP, PG_KERN_PAGE_FLAG);
	src = (char *)buff;
	dst = (char *)(VM_TMP_MAP + (vaddr % PAGE_SIZE));
	while (i < size) {
		/* Load next page */
		if ((uintptr_t)&dst[i - i_after_next_page] % PAGE_SIZE == 0) {
			vmm_unmap(VM_TMP_MAP);
			paddr = _vaddr_to_paddr(as->pd, vaddr+i);
			if (! paddr) {
				errno = EFAULT;
				vmm_map(old_tmp, VM_TMP_MAP, PG_KERN_PAGE_FLAG);
				return -1;
			}
			vmm_map(paddr, VM_TMP_MAP, PG_KERN_PAGE_FLAG);
			dst = (char *)VM_TMP_MAP;
			i_after_next_page = i;
		}
		dst[i - i_after_next_page] = src[i];
		i++;
	}
	
	vmm_unmap(VM_TMP_MAP);
	vmm_map(old_tmp, VM_TMP_MAP, PG_KERN_PAGE_FLAG);

	return i;
}

void *vmm_set_brk(vmm_aspace_t *as, uintptr_t vaddr)
{
	uintptr_t old_brk, tmp_vaddr, paddr;
	size_t new_size, prog_size, ctr;
	region_t *reg;

	/* We cannot override elf segment! */
	if (vaddr < as->elf_end) {
		errno = ENOMEM;
		return NULL;
	}
	
	old_brk = as->data_end;

	/* Initialize region for data segment */
	if (as->data_end == as->elf_end && vaddr >= as->data_end) {
		/* Align size to page border */
		new_size = vaddr - as->data_end;
		prog_size = new_size;

		if (new_size % 0x1000 != 0)
			new_size = (new_size - new_size % 0x1000) + 0x1000;

		if (! _is_area_free(as, as->data_end, new_size)) {
			errno = ENOMEM;
			return NULL;
		}

		reg = _create_region(as->data_end, new_size, VMM_RW | VMM_USER);
		_insert_region(as, reg);
		goto end;
	}

	reg = _find_region(as, as->elf_end + 1);
	if (reg == NULL)
		panic("[BUG] There is no data segment!");
	
	/* Expand data segment */
	if (vaddr > as->data_end) {
		new_size = vaddr - as->data_end;
		prog_size = new_size;

		/* Expansion can fit in already created region */
		if (vaddr <= reg->addr + reg->size)
			goto end;
		
		/* Check if we can expand region */
		if (reg->next != NULL) {
			if (new_size % 0x1000 != 0)
				new_size = (new_size - new_size % 0x1000) + 0x1000;
			
			if (reg->addr + reg->size + new_size <= reg->next->addr) {
				reg->size += new_size;
				goto end;
			} else {
				errno = ENOMEM;
				return NULL;
			}
		}
	}

	/* Shrink data segment */
	new_size = as->data_end - vaddr;
	if (as->data_end - new_size > as->elf_end) {
		/* Do not release any pages */
		if (new_size < PAGE_SIZE) {
			as->data_end -= new_size;
			return (void *)old_brk;
		}

		ctr = (size_t)(new_size / PAGE_SIZE);
		tmp_vaddr = reg->addr + reg->size;
		for (size_t i=0; i < ctr; i++) {
			paddr = vmm_unmap_from(as->pd, tmp_vaddr);
			if ((void *)paddr != NULL)
				pmm_free_frame(paddr);

			tmp_vaddr -= PAGE_SIZE;
		}
	
		reg->size -= ctr * PAGE_SIZE;	
		as->data_end -= new_size;
		return (void *)old_brk;
	} else {
		errno = ENOMEM;
		return NULL;
	}
	

end:
	as->data_end += prog_size;

	return (void *)old_brk;
}

void vmm_set_stack(vmm_aspace_t *as, uintptr_t stack_end, size_t size)
{
	as->stack_end = stack_end;
	as->stack_size = size;
}

void vmm_page_fault_handler(vmm_aspace_t *as, uintptr_t vaddr)
{
	region_t *reg;
	task_t *t;

	t = actual_task;
	reg = _find_region(as, vaddr);
	/* Incorrect address (SEGFAULT) */
	if (reg == NULL) {
		sig_send(t, SIGSEGV);
		return;
	}

	panic("NOT YET");
	/* Map new frame into target as */
	vmm_map_to(as->pd, pmm_get_frame(), vaddr, (reg->flags | PG_PRESENT));
	as->page_counter++;
}


