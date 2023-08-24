#include <stdint.h>
#include <multiboot.h>
#include <kernel/arch.h>
#include <kernel/pmm.h>
#include <dev/timer.h>

extern uint32_t kernel_paddr_end;
extern uint32_t kernel_end;

void arch_init(multiboot_info_t *mb)
{
	multiboot_module_t *mods;
	uint32_t ld_kernel_end = (uint32_t)&kernel_paddr_end;

	gdt_install();
	idt_install();
	tss_init(GDT_KERN_DATA_OFFSET, 0);
	timer_init(200);

	if (mb->mods_count > 0) {
		mods = (multiboot_module_t *) (mb->mods_addr + VM_KERN_START);
		pg_map(mb->mods_addr, mb->mods_addr + VM_KERN_START, PG_KERN_PAGE_FLAG);
		pmm_init(mb->mem_upper * 1024, 0, mods[mb->mods_count-1].mod_end);
		kernel_end = mods[mb->mods_count-1].mod_end + VM_KERN_START;
	} else {
		pmm_init(mb->mem_upper * 1024, 0, ld_kernel_end);
		kernel_end = ld_kernel_end + VM_KERN_START;
	}

	/* Map modules */
	for (size_t i=0; i < mb->mods_count; i++) {
		uint32_t mod_paddr = mods[i].mod_start;
		long mod_size = mods[i].mod_end - mods[i].mod_start;
		
		while (mod_size > 0) {
			pg_map(mod_paddr, mod_paddr + VM_KERN_START, PG_KERN_PAGE_FLAG);
			mod_paddr += FRAME_SIZE;
			mod_size -= FRAME_SIZE;
		}
	}
}
