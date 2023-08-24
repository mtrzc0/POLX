#ifndef __kernel_string_dot_H
#define __kernel_string_dot_H
#include <stddef.h>

size_t strlen(const char *str);
char *strncpy(char *dst, const char *src, size_t n);
int strcmp(const char *s1, const char *s2);
void *memset(void *s, int c, size_t n);

#endif
