#ifndef __kernel_limits_dot_H
#define __kernel_limits_dot_H

/* Maximum path string lenght for syscalls */
#define MAX_PATH_LEN 4096

/* Maximum filename length */
#define MAX_FILENAME 128

#define TERMINAL_RESOLUTION 1920
/* Maximum stdin line length that terminal will read */
#define TERMINAL_MAX_LINE_LEN TERMINAL_RESOLUTION

/* Maximum size of argv array in execve syscall */
#define MAX_ARGV 256
#endif
