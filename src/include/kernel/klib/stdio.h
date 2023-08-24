#ifndef __kernel_stdio_dot_H
#define __kernel_stdio_dot_H
#include <stdint.h>

void kprintf(const char *fmt, ...);

void panic(const char *msg);

void outb(uint16_t port, uint8_t data);
uint8_t inb(uint16_t port);

#endif
