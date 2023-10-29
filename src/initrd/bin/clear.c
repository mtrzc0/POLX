#include <dev/terminal/terminal_api.h>
#include <libc/syscalls.h>
#include <libc/stdio.h>
#include <errno.h>

int main()
{
	char clear[COMM_LEN] = {COMM_START,
				COMM_TERM_CLR,
				COMM_END};

	if (write(stdout, &clear[0], COMM_LEN) < COMM_LEN) {
		perror("Failed to clear terminal");
		exit(errno);
	}

	return 0;
}
