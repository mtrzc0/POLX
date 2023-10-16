#include <kernel_limits.h>
#include <libc/syscalls.h>
#include <libc/stdlib.h>
#include <libc/string.h>
#include <libc/stdio.h>

char *readline(const char *prompt)
{
	char *buffer;
	long len;

	buffer = (char *)malloc(TERMINAL_MAX_LINE_LEN+1);
	if (buffer == NULL) {
		perror("readline failed to allocate buffer");
		exit(1);
	}


	if (prompt != NULL)
		write(stdout, prompt, strlen(prompt));

	len = read(stdin, buffer, TERMINAL_MAX_LINE_LEN);

	if (len < 0)
		return NULL;

	buffer[len] = '\0';

	return buffer;
}
