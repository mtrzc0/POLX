#include <libc/syscalls.h>
#include <libc/string.h>
#include <libc/stdio.h>
#include <errno.h>

void _print_ussage(char *filename)
{
	printf("Ussage: %s [file] \"string\"\n", filename);
	exit(0);
}

int main(int argc, char **argv)
{
	int fd;
	size_t len;

	if (argc < 4)
		_print_ussage(argv[0]);

	fd = open(argv[1], O_WRONLY);
	if (fd < 0) {
		printf("Failed to open %s", argv[1]);
		perror("");
		exit(errno);
	}

	len = strlen(argv[2]);
	if (write(fd, argv[2], len) < 0) {
		printf("Failed to write to %s", argv[1]);
		perror("");
		exit(errno);
	}

	close(fd);

	return 0;
}
