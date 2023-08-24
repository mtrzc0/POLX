#include <i386/irq.h>
#include <kernel/klib.h>

void irq_init_PIC(void)
{
	outb(PIC1_COMM, ICW1);
	outb(PIC2_COMM, ICW1);
	
	/* Remap irqs */
	outb(PIC1_DATA, IRQ0);
	outb(PIC2_DATA, IRQ8);
	
	outb(PIC1_DATA, ICW4);
	outb(PIC2_DATA, ICW4);

	/* Enable all irqs */
	outb(PIC1_DATA, 0x0);
	outb(PIC2_DATA, 0x0);
}

void irq_set_int(uint16_t number, bool mask)
{
	uint16_t port;
	uint8_t value;

	if (number < 8) {
		port = PIC1_DATA;
	} else {
		port = PIC2_DATA;
		number -= 8;
	}

	if (mask)
		value = inb(port) | (1 << number);
	else
		value = inb(port) & ~(1 << number);	

	outb(port, value);
}
