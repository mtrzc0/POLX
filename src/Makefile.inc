ARCH = i386
TARGET = i386-unknown-elf

ASM = nasm
CC = clang
AR = llvm-ar
LD = ld.lld

ASMFLAGS = -f elf32
CFLAGS = -g -ffreestanding -march=$(ARCH) -target $(TARGET) -fno-builtin -nostdlib -m32
LDFLAGS = -nostdlib 
ARFLAGS = -rcs

INITRD_FILE = initrd.tar
