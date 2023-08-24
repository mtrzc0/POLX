#include <stdint.h>
#include <kernel/pmm.h>
#include <kernel/klib/stdio.h>

#pragma GCC diagnostic ignored "-Wint-in-bool-context"

#define _IDX_TO_ADDR(idx) (idx) * FRAME_SIZE
#define _ADDR_TO_IDX(addr) (addr) / FRAME_SIZE

/* +1 to avoid overflow before dividing MAX_MEMORY */
#define BITMAP_SIZE (MAX_MEMORY / FRAME_SIZE / 8) + 1

/* One bit for one frame */
static uint8_t pmm_bitmap[BITMAP_SIZE];
static uint32_t kernel_end_idx;
static uint32_t last_free_idx;
PMM_INFO pmm_global;

static void __bit_set(uint32_t idx, uint8_t value)
{
	uint8_t byte, bit, mask;

	byte = pmm_bitmap[idx / 8];
	bit = idx % 8;
	mask = 0x80u >> bit;

	byte &= ~mask;
	byte |= ((mask && value) << 7) >> bit;

	pmm_bitmap[idx / 8] = byte;
}

static uint8_t __bit_get(uint32_t idx)
{
	uint8_t byte, bit;

	byte = pmm_bitmap[idx / 8];
	bit = idx % 8;
	
	return byte & (0x80 >> bit);
}

#define _IDX_SET(idx) \
	__bit_set((idx), 1); \
	pmm_global.free_frames--;

#define _IDX_UNSET(idx) \
	__bit_set((idx), 0); \
	pmm_global.free_frames++; 

#define _FRAME_SET(addr) _IDX_SET(_ADDR_TO_IDX(addr))
#define _FRAME_UNSET(addr) _IDX_UNSET(_ADDR_TO_IDX(addr))

void pmm_init(uintptr_t total_memory, uintptr_t reserved_start, uintptr_t reserved_end)
{
	uintptr_t i;

	pmm_global.total_frames = total_memory / FRAME_SIZE;
	pmm_global.free_frames = pmm_global.total_frames;
	last_free_idx = 0;

	/* Mark reserved memory as used */
	for (i = reserved_start; i < reserved_end; i += FRAME_SIZE) {
		_FRAME_SET(i);
		last_free_idx++;
	}

	kernel_end_idx = last_free_idx - 1;
}

uintptr_t pmm_get_frame(void)
{
	uintptr_t new_frame_addr = 0;

	/* No more free frames */
	if (pmm_global.free_frames == 0)
		panic("[PMM] System run out of memory!");

	for (uint32_t i=last_free_idx; i < pmm_global.total_frames; i++) {
		if (! __bit_get(i)) {
			_IDX_SET(i);
			last_free_idx++;
			new_frame_addr = _IDX_TO_ADDR(i);
			break;
		}
	}

	/* Return to the beginning of the bitmap and search again */
	if (new_frame_addr == 0) {
		last_free_idx = kernel_end_idx;
		new_frame_addr = pmm_get_frame();
	}

	return new_frame_addr;
}

void pmm_free_frame(uintptr_t addr)
{
	uint32_t frame_idx = _ADDR_TO_IDX(addr);

	if (frame_idx < pmm_global.total_frames) {
		_IDX_UNSET(frame_idx);
		last_free_idx = frame_idx;
	}
}

uintptr_t pmm_get_pool(size_t frames)
{
	uintptr_t pool_start_addr;
	uint32_t pool_start_idx, pool_end_idx, frame_counter, is_first;

	pool_start_addr = 0;
	pool_start_idx = 0;
	frame_counter = 0;
	is_first = 1;

	/* Search from the bitmap beginning */
	for (uint32_t i=kernel_end_idx; i < pmm_global.total_frames; i++) {
		if (! __bit_get(i)) {
			/* First found free frame */
			if (is_first)
				pool_start_idx = i;
		
			is_first = 0;	
			frame_counter++;
			/* Check if we found everything */
			if (frame_counter == frames) {
				pool_end_idx = i;
				break;
			}
			continue;
		}

		is_first = 1;
		frame_counter = 0;
		pool_start_idx = 0;
		pool_end_idx = 0;
	}

	if (frame_counter == frames) {
		/* Mark pool as used */
		for (uint32_t i=pool_start_idx; i <= pool_end_idx; i++) {
			_IDX_SET(i);
		}
		pool_start_addr = _IDX_TO_ADDR(pool_start_idx);
	}

	return pool_start_addr;		
}

void pmm_free_pool(uintptr_t start_of_pool, size_t frames)
{
	uint32_t pool_start_idx = _ADDR_TO_IDX(start_of_pool);
	
	for (uint32_t i=0, j=pool_start_idx; i < frames; i++, j++) {
		if (j < pmm_global.total_frames) {
			_IDX_UNSET(j);
			last_free_idx = j;
		}
	}
}

