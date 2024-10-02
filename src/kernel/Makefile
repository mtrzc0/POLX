include ../conf.mk
BUILD_DIR = ../build
INCLUDE_DIR = ../include
SUB_DIRS := 	klib \
		arch/$(ARCH) \
		memory \
		cpu \
		vfs \
		ipc

CSRC := $(shell find ./$(SUB_DIRS) -type f -name \*.c)
COBJ := $(patsubst %.c, %.o, $(CSRC))
ASMSRC := $(shell find ./$(SUB_DIRS) -type f -name \*.s)
ASMOBJ := $(patsubst %.s, %.o, $(ASMSRC))


all: $(ASMOBJ) $(COBJ)
	$(CC) $(CFLAGS) -D$(ARCH) -I$(INCLUDE_DIR) -c main.c -o $(BUILD_DIR)/main.o

%.o: %.c
	$(CC) $(CFLAGS) -D$(ARCH) -I$(INCLUDE_DIR) -c $< -o $(BUILD_DIR)/$(notdir $@)

%.o: %.s
	$(ASM) $(ASMFLAGS) $< -o $(BUILD_DIR)/$(notdir $@)
