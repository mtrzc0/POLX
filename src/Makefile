include Makefile.inc
BUILD_DIR = ./build
BIN_DIR = ./out
BIN_NAME = os.bin
ISO_DIR = $(BUILD_DIR)/iso
ISO_NAME = os.iso
MODULES_DIRS = 	./kernel \
		./dev \
		./libc \
		./initrd

all: os 
.PHONY: os build_modules clean run

os: build_modules
	$(CC) $(CLFAGS) $(LDFLAGS) -T linker.ld -o $(BIN_DIR)/$(BIN_NAME) \
	$(wildcard $(BUILD_DIR)/*.o)

build_modules:
	for dir in $(MODULES_DIRS); do \
		make -C $$dir; \
	done

clean:
	rm -Rf $(BUILD_DIR)/*
	rm -Rf $(BIN_DIR)/*

iso:
	mkdir -p $(ISO_DIR)/boot/grub
	cp ./grub.cfg $(ISO_DIR)/boot/grub
	cp $(BIN_DIR)/$(BIN_NAME) $(ISO_DIR)/boot
	grub-mkrescue -o $(BIN_DIR)/$(ISO_NAME) $(ISO_DIR)

run:
	qemu-system-i386 -m 512M -kernel $(BIN_DIR)/$(BIN_NAME)

run-initrd:
	qemu-system-i386 -m 512M -kernel $(BIN_DIR)/$(BIN_NAME) -initrd $(BIN_DIR)/$(INITRD_FILE)

run-iso:
	qemu-system-i386 -cdrom $(BIN_DIR)/$(ISO_NAME)

gdb:
	qemu-system-i386 -m 512M -s -S -kernel $(BIN_DIR)/$(BIN_NAME)

gdb-initrd:
	qemu-system-i386 -m 512M -s -S -kernel $(BIN_DIR)/$(BIN_NAME) -initrd $(BIN_DIR)/$(INITRD_FILE)

debug:
	qemu-system-i386 -m 512M -monitor stdio -d int -kernel $(BIN_DIR)/$(BIN_NAME) -initrd $(BIN_DIR)/$(INITRD_FILE)
