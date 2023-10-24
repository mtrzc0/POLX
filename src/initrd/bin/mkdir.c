#include <libc/syscalls.h>
#include <libc/stdio.h>
#include <errno.h>

void _print_ussage(char *filename)
{
	printf("Ussage: %s [FILENAME]\n", filename);
	exit(0);
}

int main(int argc, char **argv)
{
	if (argc < 3)
		_print_ussage(argv[0]);

	if (mkdir(argv[1], 0) < 0) {
		printf("%s: failed to create directory %s", argv[0], argv[1]);
		perror("");
		exit(errno);
	}

	return 0;
}
