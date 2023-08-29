#ifndef __timer_dot_H
#define __timer_dot_H

#include <stdint.h>

#define BASE_FREQUENCY 1193180

#define CH0		0x40
#define CH1		0x41
#define CH2		0x42
#define TIMER_COMM	0x43


void timer_init(uint32_t freq);
int timer_isr(void);

#endif
