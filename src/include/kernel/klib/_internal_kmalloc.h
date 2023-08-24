#ifndef __kernel_int_kmalloc_dot_H
#define __kernel_int_kmalloc_dot_H

#include <stddef.h>

// TODO add 1 to SIZEBINS after implementing tasks
#define SIZEBINS 5
#define BINS_QUANTITY SIZEBINS + 1
#define SORTEDBIN_IDX BINS_QUANTITY - 1

struct chunk_header {
#ifdef i386
	size_t size 	 : 30;
#endif
	size_t free	 : 1; // Set when chunk was released
	size_t prev_free : 1; // Set when previous chunk was released
	size_t prev_size;     // Used when prev_free is set

	struct chunk_header *prev, *next; 
};
typedef struct chunk_header* chunkptr_t;

struct bin_info {
	size_t stored_size; // Indicates size of stored chunks
	size_t counter;
	chunkptr_t first;
	chunkptr_t last;
};
typedef struct bin_info bin_t;

struct malloc_state {
	int is_initialized;
	bin_t bins[BINS_QUANTITY];
	chunkptr_t void_chunk;
};

#define CHUNK_ALIGNMENT 0x10
#define MINIMAL_CHUNK_SIZE sizeof(struct chunk_header)
#define CHUNK_DATA_OFFSET (2*sizeof(size_t))

#define chunk2mem(p)   ((void*)((char*)(p) + CHUNK_DATA_OFFSET))
#define mem2chunk(mem) ((chunkptr_t)((char*)(mem) - CHUNK_DATA_OFFSET))

#endif

