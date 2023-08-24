#include <stdint.h>
#include <kernel/klib/string.h>
#include <i386/tss.h>

tss_entry_t tss;

void tss_init(uint32_t gdt_stack_desc, uint32_t esp)
{
	// Ensure that tss is initialized with 0
	memset(&tss, 0, sizeof(tss_entry_t));
	tss.ss0 = gdt_stack_desc;
	tss.esp0 = esp;
	tss.io_map = sizeof(tss_entry_t);
	tss_flush();
}


void tss_set_stack(uint32_t stack)
{
	tss.esp0 = stack;
}
