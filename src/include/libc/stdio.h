#ifndef __libc_stdio_dot_H
#define __libc_stdio_dot_H

#include <libc/syscalls.h>

int printf(const char *restrict format, ...);

void perror(const char *s);

char *readline(const char *prompt);

#endif
