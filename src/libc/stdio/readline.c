#include <kernel_limits.h>
#include <libc/syscalls.h>
#include <libc/string.h>

int readline(const char *prompt, char *buffer)
{
	if (prompt != NULL)
		write(stdout, prompt, strlen(prompt));

	if (read(stdin, buffer, TERMINAL_MAX_LINE_LEN) < 0)
		return -1;

	return 0;
}
