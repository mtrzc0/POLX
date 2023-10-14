#ifndef __isr_dot_H
#define __isr_dot_H

#include <stdint.h>

#define ENABLE_INTERRUPTS \
	__asm__ __volatile__("sti":::)

typedef struct {
	uint32_t int_number;
	uint32_t err_code;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
	uint32_t esp;
	uint32_t ss;
} int_frame_t;

typedef struct {
	uint32_t ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
} int_regs_t;

void general_handler(int_frame_t *int_frame, int_regs_t *task_regs);

#endif
