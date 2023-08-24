include ../../Makefile.inc
BUILD_DIR = ../../build
INCLUDE_DIR = ../../include

CSRC := $(shell find ./ -type f -name \*.c)
COBJ := $(patsubst %.c, %.o, $(CSRC))

all: $(COBJ)

%.o: %.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $(BUILD_DIR)/$(notdir $@)
