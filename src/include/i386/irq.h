#ifndef __irq_dot_H
#define __irq_dot_H

#include <stdint.h>
#include <stdbool.h>

#define PIC1_COMM	0x20
#define PIC1_DATA	0x21
#define PIC2_COMM	0xa0
#define PIC2_DATA	0xa1

#define PIC_EOI		0x20

#define ICW1		0x11
#define ICW4		0x1

#define IRQ0		0x20
#define IRQ8		0x28

#define IRQ_QUANTITY	0x10

void irq_init_PIC(void);

void irq_set_int(uint16_t number, bool mask);

#define irq_mask_int(number) \
	irq_set_int(number, 1)

#define irq_unmask_int(number) \
	irq_set_int(number, 0)

#endif
