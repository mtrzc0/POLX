SHELL = /bin/bash

# compile with debug symbols
# y or n
DEBUG = n

# build target architecture
ARCH = i386
TARGET = i386-unknown-elf

# tools
ASM = nasm
CC = clang
AR = llvm-ar
LD = ld.lld

# flags
ASMFLAGS = -f elf32
CFLAGS = -ffreestanding -march=$(ARCH) -target $(TARGET) -fno-builtin -nostdlib -m32
LDFLAGS = -nostdlib -z noexecstack -fuse-ld=lld
ARFLAGS = -rcs
INITRD_FILE = initrd.tar

ifeq ($(DEBUG), y)
    CFLAGS += -g
endif
