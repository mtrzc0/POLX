#ifndef __gdt_dot_H
#define __gdt_dot_H

#include <stdint.h>

#define GDT_ENTRY_SIZE       0x8
#define GDT_KERN_CODE_OFFSET GDT_ENTRY_SIZE
#define GDT_KERN_DATA_OFFSET GDT_ENTRY_SIZE * 2
#define GDT_USER_CODE_OFFSET GDT_ENTRY_SIZE * 3
#define GDT_USER_DATA_OFFSET GDT_ENTRY_SIZE * 4

/* GDT entry

	0		15 16		   31
	+-----------------+-----------------+
	|    limit_low    |    base_low     |
	+-----------------+-----------------+
	|  base  | access | flags  | base_h |
	+--------+--------+--------+--------+
	32     39 40    47 48   55 56      63 

flags field division details

	0		 4 5		    7
	+-----------------+-----------------+
	|      limit      |      flags      |
	+-----------------+-----------------+
*/

/* Access field consts */
#define GDT_ACS_PRESENT 0x80
#define GDT_ACS_CPL_1	0x20
#define GDT_ACS_CPL_2	0x40
#define GDT_ACS_CPL_3	0x60
#define GDT_ACS_EXEC	0x8
#define GDT_ACS_RW	0x2
#define GDT_TSS_AVAIL	0x9

#define GDT_BASE_ACS GDT_ACS_PRESENT | 0x10

#define GDT_ACS_KERN_CODE GDT_BASE_ACS | GDT_ACS_EXEC | GDT_ACS_RW
#define GDT_ACS_KERN_DATA GDT_BASE_ACS | GDT_ACS_RW
#define GDT_ACS_USER_CODE GDT_ACS_KERN_CODE | GDT_ACS_CPL_3
#define GDT_ACS_USER_DATA GDT_ACS_KERN_DATA | GDT_ACS_CPL_3
#define GDT_ACS_TSS GDT_ACS_PRESENT | GDT_ACS_EXEC | GDT_TSS_AVAIL

#define GDT_BASE_FLG 0xc

struct gdt_entry {
	uint64_t limit_low	: 16;
	uint64_t base_low	: 16;
	uint64_t base_mid	:  8;
	uint64_t access		:  8;
	uint64_t limit_high	:  4;
	uint64_t flags		:  4;
	uint64_t base_high	:  8;
} __attribute__((packed));
typedef struct gdt_entry gdt_entry_t;

struct gdt_ptr {
	uint16_t size;
	uint32_t base;
} __attribute__((packed));
typedef struct gdt_ptr gdt_ptr_t;

void gdt_load(gdt_ptr_t *ptr);
void gdt_install(void);

#endif
