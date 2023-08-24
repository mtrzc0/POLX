#ifndef __idt_dot_H
#define __idt_dot_H

#include <stdint.h>

#define IDT_MAX_ENTRIES	   256
#define IDT_ENTRY_SIZE	   0x8
#define IDT_CPU_EXCEPTIONS 32
/* IDT entry

	0		15 16		   31
	+-----------------+-----------------+
	|   offset_low    |	selector    |
	+-----------------+-----------------+
	| attrs  | reserv |   offset_high   |
	+--------+--------+-----------------+
        32     39 40    47 48              63

attributes details

	       BASE_ATTR
        7		       0
	+---+-----+---+--------+
	| P | CPL | 0 |  TYPE  |
	+---+-----+---+--------+
          1   00    0    0000
*/

#define IDT_BASE_ATTR 0x80

#define IDT_CPL_1 0x20
#define IDT_CPL_2 0x40
#define IDT_CPL_3 0x60

#define IDT_INT_GATE  0xe
#define IDT_TRAP_GATE 0xf

#define IDT_EXCEPTION_DESCRIPTOR IDT_BASE_ATTR | IDT_TRAP_GATE
#define IDT_IRQ_DESCRIPTOR IDT_BASE_ATTR | IDT_INT_GATE
#define IDT_SYSCALL_DESCRIPTOR IDT_BASE_ATTR | IDT_INT_GATE | IDT_CPL_3

struct idt_entry {
	uint64_t offset_low	: 16;
	uint64_t selector	: 16;
	uint64_t reserved	:  8;
	uint64_t attributes	:  8;
	uint64_t offset_high	: 16;
} __attribute__((packed));
typedef struct idt_entry idt_entry_t;

struct idt_ptr {
	uint16_t size;
	uint32_t base;
} __attribute__((packed));
typedef struct idt_ptr idt_ptr_t;

void idt_load(idt_ptr_t *ptr);
void idt_install(void);

#endif
