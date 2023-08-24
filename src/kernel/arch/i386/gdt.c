#include <stdint.h>
#include <i386/gdt.h>
#include <i386/tss.h>

#define GDT_ENTRIES 6

gdt_entry_t gdt[GDT_ENTRIES];
gdt_ptr_t gp;

extern tss_entry_t tss;

static void gdt_fill_entry(uint32_t entry, uint32_t limit, uint32_t base, uint8_t access, uint8_t flags)
{
	gdt[entry].limit_low  = (limit & 0xffff);
	gdt[entry].limit_high = ((limit >> 16) & 0xf);

	gdt[entry].base_low  = (base & 0xffff);
	gdt[entry].base_mid  = ((base >> 16) & 0xff);
	gdt[entry].base_high = ((base >> 24) & 0xff);

	gdt[entry].access = access;

	gdt[entry].flags = (flags & 0xf);
}

void gdt_install(void)
{
	gp.size = (GDT_ENTRY_SIZE * GDT_ENTRIES) - 1;
	gp.base = (uint32_t)&gdt;

	gdt_fill_entry(0, 0, 0, 0, 0);
	gdt_fill_entry(1, 0xffffffff, 0x0, GDT_ACS_KERN_CODE, GDT_BASE_FLG);    // Kernel code
	gdt_fill_entry(2, 0xffffffff, 0x0, GDT_ACS_KERN_DATA, GDT_BASE_FLG);    // Kernel data
	gdt_fill_entry(3, 0xffffffff, 0x0, GDT_ACS_USER_CODE, GDT_BASE_FLG);    // User code
	gdt_fill_entry(4, 0xffffffff, 0x0, GDT_ACS_USER_DATA, GDT_BASE_FLG);    // User data
	gdt_fill_entry(5, sizeof(tss_entry_t), (uint32_t)&tss, GDT_ACS_TSS, 0); // TSS segment

	gdt_load(&gp);
}
