#include <stddef.h>

void *memcpy(void *dest, void *src, size_t count) 
{
    char *d = (char *) dest;
    char *s = (char *) src;
    for (size_t i=0; i < count; i++) {
        d[i] = s[i];    
    }
}
