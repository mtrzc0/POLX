#ifndef __arch_dot_H
#define __arch_dot_H

#include <multiboot.h>

#ifdef i386

#include <i386/context_switch.h>
#include <i386/gdt.h>
#include <i386/idt.h>
#include <i386/isr.h>
#include <i386/irq.h>
#include <i386/paging.h>
#include <i386/task_regs.h>
#include <i386/tss.h>
#include <i386/vmmap.h>

#define PAGE_SIZE 4096
#define MAX_MEMORY UINT32_MAX

#endif

void arch_init(multiboot_info_t *mb);

#endif
