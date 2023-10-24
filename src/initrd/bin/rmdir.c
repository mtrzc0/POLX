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

	if (rmdir(argv[1]) < 0) {
		printf("%s: failed to remove %s", argv[0], argv[1]);
		if (errno == EEXIST)
			printf(": Directory not empty\n");
		else
			perror("");
		exit(errno);
	}

	return 0;
}
