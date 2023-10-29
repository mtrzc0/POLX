#include <libc/stdio.h>

void _print_ussage(char *filename)
{
	printf("Ussage: %s \"string\"\n", filename);
	exit(0);
}

int main(int argc, char **argv)
{
	if (argc < 3) {
		_print_ussage(argv[0]);
	}

	printf("%s\n", argv[1]);
	
	return 0;
}
