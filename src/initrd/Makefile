include ../conf.mk

BUILD_DIR = ../build/initrd
BIN_DIR = $(BUILD_DIR)/bin
OUT_DIR = ../out
INCLUDE_DIR = ../include

CSRC := $(shell find ./bin -type f -name \*.c)
COUT := $(patsubst %.c, %, $(CSRC))

.PHONY: tree

all: tree $(COUT)
	@tar -C $(BUILD_DIR) -cf $(OUT_DIR)/$(INITRD_FILE) ./

# TODO: refactor building initrd
%: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -I$(INCLUDE_DIR) $< ../out/libc.a -o $(BIN_DIR)/$(notdir $@)

tree:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)
	@find ./* -maxdepth 0 \! -name 'bin' -type d -exec cp -r {} $(BUILD_DIR) \;
	@find ./* -maxdepth 0 \! -name 'Makefile' -type f -exec cp {} $(BUILD_DIR) \;
