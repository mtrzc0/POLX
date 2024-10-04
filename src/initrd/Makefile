include ../conf.mk

BUILD_DIR = ../build/initrd
TEMP_DIR = ../build/temp
BIN_DIR = $(BUILD_DIR)/bin
OUT_DIR = ../out
INCLUDE_DIR = ../include

CSRC := $(notdir $(shell find ./bin -type f -name \*.c))
COBJ := $(patsubst %.c, $(TEMP_DIR)/%.o, $(CSRC))
COUT := $(patsubst %.c, $(BIN_DIR)/%, $(CSRC))

.PHONY: tree

all: tree $(COBJ) $(COUT)
	@tar -C $(BUILD_DIR) -cf $(OUT_DIR)/$(INITRD_FILE) ./

$(TEMP_DIR)/%.o: bin/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(BIN_DIR)/%: $(TEMP_DIR)/%.o
	$(LD) $(LDFLAGS) $< ../out/libc.a -o $@

tree:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(TEMP_DIR)
	@find ./* -maxdepth 0 \! -name 'bin' -type d -exec cp -r {} $(BUILD_DIR) \;
	@find ./* -maxdepth 0 \! -name 'Makefile' -type f -exec cp {} $(BUILD_DIR) \;