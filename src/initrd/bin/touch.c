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
	int fd;

	if (argc < 3)
		_print_ussage(argv[0]);

	fd = open(argv[1], O_RDONLY | O_CREAT);
	if (fd < 0) {
		printf("%s: cannot access %s", argv[0], argv[1]);
		perror("");
		exit(errno);	
	}

	close(fd);
	return 0;
}
