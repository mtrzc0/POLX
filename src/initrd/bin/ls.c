#include <libc/syscalls.h>
#include <libc/stdio.h>
#include <errno.h>

void _print_ussage(char *filename)
{
	printf("Ussage: %s [FILE]\n", filename);
	exit(0);
}

int main(int argc, char **argv)
{
	struct dirent dent;
	int fd, readdir_ret;

	if (argc < 3)
		_print_ussage(argv[0]);

	fd = open(argv[1], O_DIRECTORY);
	if (fd < 1) {
		printf("%s: cannot access %s", argv[0], argv[1]);
		perror("");
		exit(2);
	}

	readdir_ret = readdir(fd, &dent);
	while (readdir_ret > 0) {
		if (dent.name[0] != '\0')
			printf("%s ", dent.name);
		readdir_ret = readdir(fd, &dent);
	}

	if (readdir_ret < 0) {
		printf("%s: %s", argv[0], argv[1]);
		perror("");
		exit(errno);
	}
	printf("\n");

	return 0;
}
