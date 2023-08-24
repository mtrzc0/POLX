#include <stdint.h>
#include <i386/gdt.h>
#include <i386/idt.h>
#include <i386/irq.h>

idt_entry_t idt[IDT_MAX_ENTRIES];
idt_ptr_t ip;
uint32_t current = 0;

extern uint32_t isr_vectors[];

static void _idt_fill_entry(uint32_t offset, uint16_t selector, uint8_t attrs)
{
	if (current < IDT_MAX_ENTRIES) {
		idt[current].offset_low  = (offset & 0xffff);
		idt[current].offset_high = ((offset >> 16) & 0xffff);

		idt[current].selector = selector;
	
		idt[current].reserved = 0;
	
		idt[current].attributes = attrs;

		current++;
	}
}

static void _cpu_exceptions_init(void)
{
	for (uint32_t i=0; i < IDT_CPU_EXCEPTIONS; i++) {
		_idt_fill_entry(isr_vectors[i], GDT_KERN_CODE_OFFSET, IDT_EXCEPTION_DESCRIPTOR);
	}
}

static void _irqs_init(void)
{
	irq_init_PIC();
	
	for (uint32_t i=IRQ0; i < IRQ0 + 16; i++) {
		_idt_fill_entry(isr_vectors[i], GDT_KERN_CODE_OFFSET, IDT_IRQ_DESCRIPTOR);
	}
}

void idt_install(void)
{
	_cpu_exceptions_init();
	_irqs_init();

	/* Syscalls trigger */
	_idt_fill_entry(isr_vectors[48], GDT_KERN_CODE_OFFSET, IDT_SYSCALL_DESCRIPTOR);

	ip.size = (IDT_ENTRY_SIZE * current) - 1;
	ip.base = (uint32_t)&idt;

	idt_load(&ip);
	__asm__ volatile ("sti");
}
