ARCH = i386

ASM = nasm
CC = i686-elf-gcc
AR = i686-elf-ar

ASMFLAGS = -f elf32
CFLAGS = -g -Wall -ffreestanding -O0 -Wextra
LDFLAGS = -nostdlib -lgcc
ARFLAGS = -rcs

INITRD_FILE = initrd.tar
