include ../Makefile.inc
LIBC_OUT = ../out/libc.a
BUILD_DIR = ../build/libc
INCLUDE_DIR = ../include
SUB_DIRS := arch/$(ARCH) \
		stdio	\
		string	\

CSRC := $(shell find ./$(SUB_DIRS) -type f -name \*.c)
COBJ := $(patsubst %.c, %.o, $(CSRC))
ASMSRC := $(shell find ./$(SUB_DIRS) -type f -name \*.s)
ASMOBJ := $(patsubst %.s, %.o, $(ASMSRC))

.PHONY: tree

all: tree $(ASMOBJ) $(COBJ)
	$(AR) $(ARFLAGS) $(LIBC_OUT) $(shell find $(BUILD_DIR) -type f -name \*.o)

%.o: %.c
	$(CC) $(CFLAGS) -D$(ARCH) -I$(INCLUDE_DIR) -c $< -o $(BUILD_DIR)/$(notdir $@)

%.o: %.s
	$(ASM) $(ASMFLAGS) $< -o $(BUILD_DIR)/$(notdir $@)

tree:
	@mkdir $(BUILD_DIR)
