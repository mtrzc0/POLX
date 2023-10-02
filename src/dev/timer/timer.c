#include <dev/timer.h>
#include <kernel/klib.h>
#include <kernel/task.h>

static uint32_t tick = 0;

void timer_init(uint32_t freq)
{
	uint32_t div;
	uint8_t low, high;

	div = BASE_FREQUENCY / freq;
	low = (uint8_t)(div & 0xff);
	high = (uint8_t)((div >> 8) & 0xff);

	outb(TIMER_COMM, 0x36);
	outb(CH0, low);
	outb(CH0, high);
}

int timer_isr(void)
{
	tick++;
	//kprintf("tick\n");	
	if (tick % 10 == 0)
		return 1;
	
	return 0;
}
