#ifndef __libc_stdio_dot_H
#define __libc_stdio_dot_H

#include <libc/syscalls.h>

int printf(const char *restrict format, ...);

/* Save line into buffer and return 0 if success or -1 if failed */
int readline(const char *prompt, char *buffer);

#endif
