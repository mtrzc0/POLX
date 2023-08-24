OUTPUT_FORMAT(elf32-i386)
ENTRY(start)

PADDR_BASE = 0x100000;
VADDR_BASE = 0xC0000000;

SECTIONS
{
	/* Phisical address space */
	. = PADDR_BASE;
	kernel_paddr_start = .;

	.boot : ALIGN(4k) {
		*(.multiboot)
		*(.bootstrap)

		/* Align to page border */
		. = ALIGN(4k);
	}

	/* Virtual address space */
	. = . + VADDR_BASE;
	kernel_vaddr_start = .;

	.text : AT(ADDR(.text) - VADDR_BASE) ALIGN(4k) {
		*(.text.*)
		*(.text)
	}

	.rodata : AT(ADDR(.rodata) - VADDR_BASE) ALIGN(4k) {
		*(.rodata.*)
		*(.rodata)
	}

	.data : AT(ADDR(.data) - VADDR_BASE) ALIGN(4k) {
		*(.data.*)
		*(.data)
	}

	.bss : AT(ADDR(.bss) - VADDR_BASE) ALIGN(4k) {
		*(.bss.*)
		*(.bss)
		*(COMMON)
	}
	kernel_vaddr_end = .;
	kernel_paddr_end = . - VADDR_BASE;
}
