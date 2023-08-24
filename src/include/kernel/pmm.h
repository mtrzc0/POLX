#ifndef __pmm_dot_H
#define __pmm_dot_H

#include <stdint.h>
#include <stddef.h>
#include <kernel/arch.h>

#define FRAME_SIZE PAGE_SIZE

typedef struct {
	uint32_t total_frames;
	uint32_t free_frames;
} PMM_INFO;

void pmm_init(uintptr_t total_memory, uintptr_t reserved_start, uintptr_t reserved_end);

uintptr_t pmm_get_frame(void);
void pmm_free_frame(uintptr_t paddr);

uintptr_t pmm_get_pool(size_t frames);
void pmm_free_pool(uintptr_t start_of_pool, size_t frames);

#endif
