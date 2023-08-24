#include <stdint.h>

void outb(uint16_t port, uint8_t data)
{
	__asm__("out %%al, %%dx" : : "a" (data), "d" (port));
}

uint8_t inb(uint16_t port)
{
	uint8_t ret;
	__asm__("in %%dx, %%al" : "=a" (ret) : "d" (port));
	return ret;
}
