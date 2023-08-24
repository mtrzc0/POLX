#include <stddef.h>

void *memset(void *s, int c, size_t n)
{
	char *ptr = (char *) s;
	for (size_t i=0; i < n; i++)
		ptr[i] = c;
	return s;
}
