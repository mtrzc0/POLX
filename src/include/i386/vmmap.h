#ifndef __vmmap_dot_H
#define __vmmap_dot_H

#include <stdint.h>

extern uint32_t kernel_end;

/* User process address space */
#define VM_USER_START 		0x00001000
#define VM_USER_STACK		0xbffff000
#define VM_USER_END		0xc0000000

/* Kernel address space */
#define VM_KERN_START		0xc0000000

#define VM_KMALLOC_START	kernel_end + 0x1000
#define VM_KMALLOC_END		0xd0000000

#define VM_VMALLOC_START	0xd0000000
#define VM_VMALLOC_END		0xdffff000

#define VM_KERN_END		0xe0000000

#define VM_TMP_MAP		0xff7fe000
#define VM_TMP2_MAP		0xff7ff000

#define VM_EXPAGE_TABLES 	0xff800000
#define VM_EXPAGE_DIR 		0xffbff000

#define VM_PAGE_TABLES		0xffc00000
#define VM_PAGE_DIR		0xfffff000

#endif
