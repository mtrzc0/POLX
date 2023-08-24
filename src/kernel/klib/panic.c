#include <kernel/klib/stdio.h>

void panic(const char *msg)
{
	kprintf("%s", msg);
	__asm__ volatile ("cli; hlt");
}
