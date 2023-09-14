#include <stdint.h>
#include <i386/paging.h>
#include <i386/vmmap.h>
#include <kernel/pmm.h>
#include <kernel/vmm.h>
#include <kernel/task.h>
#include <kernel/klib.h>

#define _IS_ENTRY_PRESENT(paddr) (paddr) & PG_PRESENT
#define _IS_FLAG_SET(paddr, flag) (paddr) & (flag)
#define _CREATE_PD_ENTRY(paddr) ((paddr) & PG_ALIGN) | PG_PRESENT | PG_RW
#define _GET_PD_IDX(addr) ((addr) >> 22)
#define _GET_PT_IDX(addr) ((addr) >> 12)

#define _INVLPG(vaddr) \
	__asm__ __volatile__("invlpg (%0)" : : "a" (vaddr & PG_ALIGN))

extern task_t *actual_task;
extern uint32_t kern_pd[];
uint32_t kern_pd_paddr = (uint32_t)(&kern_pd[0]) - 0xc0000000;
static uint32_t *current_pd   = (uint32_t *)VM_PAGE_DIR;
static uint32_t *current_pts  = (uint32_t *)VM_PAGE_TABLES;
static uint32_t *external_pd  = (uint32_t *)VM_EXPAGE_DIR;
static uint32_t *external_pts = (uint32_t *)VM_EXPAGE_TABLES;

static inline __attribute__((always_inline))
uint32_t _get_pd_addr(void)
{
	uint32_t paddr;
	__asm__ __volatile__("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(paddr)::"%eax");
	
	return paddr & PG_ALIGN;
}

static inline __attribute__((always_inline))
uint32_t _get_cr2_value(void)
{
	uint32_t cr2;
	__asm__ __volatile__("movl %%cr2, %%eax; movl %%eax, %0;":"=m"(cr2)::"%eax");

	return cr2;
}

static uint32_t _set_expd(uint32_t pd)
{
	uint32_t old;

	old = current_pd[_GET_PD_IDX(VM_EXPAGE_DIR)];
	current_pd[_GET_PD_IDX(VM_EXPAGE_DIR)] = (pd | PG_PRESENT | PG_RW);
	_INVLPG(VM_EXPAGE_DIR);
	
	/* Flush external_pts mappings from tlb */
	for (uintptr_t i=VM_EXPAGE_TABLES; i < VM_EXPAGE_DIR; i += PAGE_SIZE)
		_INVLPG(i);

	return old;
}

static uint32_t _get_page(uint32_t vaddr)
{
	vaddr &= PG_ALIGN;

	if (_IS_ENTRY_PRESENT(current_pd[_GET_PD_IDX(vaddr)]))
		if (_IS_ENTRY_PRESENT(current_pts[_GET_PT_IDX(vaddr)]))
			return current_pts[_GET_PT_IDX(vaddr)];
	
	return 0;
}

static uint32_t _get_expage(uint32_t vaddr)
{
	vaddr &= PG_ALIGN;

	if (_IS_ENTRY_PRESENT(external_pd[_GET_PD_IDX(vaddr)]))
		if (_IS_ENTRY_PRESENT(external_pts[_GET_PT_IDX(vaddr)]))
			return external_pts[_GET_PT_IDX(vaddr)];

	return 0;
}

static void _clone_kernel_tables(uint32_t *dst_pd_vaddr)
{
	for (uint32_t i=_GET_PD_IDX(VM_KERN_START); i < _GET_PD_IDX(VM_KERN_END); i++)
		dst_pd_vaddr[i] = kern_pd[i];
}

void pg_switch_pd(uint32_t new)
{
	__asm__ __volatile__("movl %0, %%eax; movl %%eax, %%cr3;":: "r"(new): "%eax");
}

void pg_map(uint32_t paddr, uint32_t vaddr, uint32_t flags)
{
	/* Page table does not exist */
	if (! _IS_ENTRY_PRESENT(current_pd[_GET_PD_IDX(vaddr)])) {
		current_pd[_GET_PD_IDX(vaddr)] = _CREATE_PD_ENTRY(pmm_get_frame());
	}
	
	current_pts[_GET_PT_IDX(vaddr)] = (paddr & PG_ALIGN) | flags;
}

void pg_map_to(uint32_t pd, uint32_t paddr, uint32_t vaddr, uint32_t flags)
{
	uint32_t old_pd;

	old_pd = _set_expd(pd);

	/* Create page table if not exist */
	if (! _IS_ENTRY_PRESENT(external_pd[_GET_PD_IDX(vaddr)])) {
		if (_IS_FLAG_SET(flags, PG_USER))
			external_pd[_GET_PD_IDX(vaddr)] = _CREATE_PD_ENTRY(pmm_get_frame()) | PG_USER;
		else
			external_pd[_GET_PD_IDX(vaddr)] = _CREATE_PD_ENTRY(pmm_get_frame());
	}

	external_pts[_GET_PT_IDX(vaddr)] = (paddr & PG_ALIGN) | flags;
	_set_expd(old_pd);
}

uint32_t pg_duplicate_page(uint32_t pd, uint32_t vaddr)
{
	uint32_t old_pd, old_tmp, old_tmp2; 
	uint32_t new_frame, old_frame;
	char *tmp, *tmp2;

	old_pd = _set_expd(pd);

	old_frame = _get_expage(vaddr);
	if (old_frame == 0) {
		_set_expd(old_pd);
		return 0;
	}
	new_frame = pmm_get_frame();

	old_tmp = pg_unmap(VM_TMP_MAP);
	old_tmp2 = pg_unmap(VM_TMP2_MAP);

	pg_map(old_frame, VM_TMP_MAP, PG_KERN_PAGE_FLAG);
	pg_map(new_frame, VM_TMP2_MAP, PG_KERN_PAGE_FLAG);
	tmp = (char *)(VM_TMP_MAP);
	tmp2 = (char *)(VM_TMP2_MAP);

	for (uint32_t i=0; i < PAGE_SIZE; i++)
		tmp2[i] = tmp[i];

	pg_unmap(VM_TMP_MAP);
	pg_unmap(VM_TMP2_MAP);
	pg_map(old_tmp, VM_TMP_MAP, PG_KERN_PAGE_FLAG);
	pg_map(old_tmp2, VM_TMP2_MAP, PG_KERN_PAGE_FLAG);

	_set_expd(old_pd);

	return new_frame;
}

uint32_t pg_unmap(uint32_t vaddr)
{
	uint32_t paddr;

	paddr = _get_page(vaddr);	
	if (paddr) {
		current_pts[_GET_PT_IDX(vaddr)] = 0;
		_INVLPG(vaddr);
	} else {
		return 0;
	}

	return paddr | PG_ALIGN;
}

uint32_t pg_unmap_from(uint32_t pd, uint32_t vaddr)
{
	uint32_t old_pd, paddr;

	old_pd = _set_expd(pd);
	paddr = _get_expage(vaddr);
	if (paddr) {
		external_pts[_GET_PT_IDX(vaddr)] = 0;
		/* 
		   Do NOT use _INVLPG here, bc it will 
		   remove vaddr tlb entry of current pd 
		*/
	} else {
		return 0;
	}

	_set_expd(old_pd);
	return paddr | PG_ALIGN;
}

uint32_t pg_get_frame_from(uint32_t pd, uint32_t vaddr)
{
	uint32_t old_pd, paddr;

	old_pd = _set_expd(pd);

	paddr = _get_expage(vaddr);
	_set_expd(old_pd);

	return paddr;
}

uint32_t pg_create_directory(uint32_t parent_pd)
{
	uint32_t new_pd, old_tmp;
	uint32_t *pd_map;

	(void)parent_pd;

	/* Save old value */
	old_tmp = pg_unmap(VM_TMP_MAP);

	/* Allocate frame for new PD */
	new_pd = pmm_get_frame();
	pg_map(new_pd, VM_TMP_MAP, PG_KERN_PAGE_FLAG);
	memset((void *)VM_TMP_MAP, 0, FRAME_SIZE);

	/* Make it recursive */
	pd_map = (uint32_t *)VM_TMP_MAP;
	pd_map[1023] = ((new_pd & PG_ALIGN) | PG_PRESENT | PG_RW);

	_clone_kernel_tables(pd_map);
	
	/* Restore old value */
	pg_map(old_tmp, VM_TMP_MAP, PG_KERN_PAGE_FLAG);
	
	return new_pd;
}

void pg_destroy_directory(uint32_t pd)
{
	/* VMM is responsible for destroing user address space */
	pmm_free_frame(pd);
}

void pg_page_fault_handler(uint32_t err_code)
{
	uint32_t vaddr;
	unsigned int vmm_err;

	vaddr = _get_cr2_value();

	/* Page fault in user mode */
	if (err_code & PG_USER) {
		/* Translate err_code into VMM_ERR codes */
		vmm_err = 0;
		if (! (err_code & PG_PRESENT))
			vmm_err |= VMM_ERR_PR;
		if (err_code & PG_RW)
			vmm_err |= VMM_ERR_WR;
		else
			vmm_err |= VMM_ERR_RD;

		vmm_page_fault_handler(actual_task->aspace, vaddr, vmm_err);
	} else {
	/* Page fault in kernel mode */
		if (vaddr < 0x1000)
			panic("[BUG] NULL address ussage!");
		
		if (! (err_code & PG_PRESENT)) {
			pg_map(pmm_get_frame(), vaddr, PG_KERN_PAGE_FLAG);
		} else {
			kprintf("\nPAGE FAULT ERROR NOT IMPLEMENTED YET\n");
			kprintf("CR2: %x, ERROR CODE: %x", _get_cr2_value(), err_code);
			panic("");
		}
	}
}
