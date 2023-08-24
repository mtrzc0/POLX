# TODO 
- [x] uncomment kfree after implementing (kmalloc.c)
- [ ] uncomment taskbin after implementing tasks (kmalloc.c, \_internal_malloc.h)
- [ ] implement merge_adjacent_chunks function (kmalloc.c)

#  ***All memory pieces are chunks***
[kmalloc](#malloc) and [kfree](#free) will manage the address space indicated by _<kernel/arch.h>_. That memory will be divided into ordered pieces called chunks. Chunks will be indicated by their [headers](#chunk_header) which contain required informations, the chunk size will be aligned to 16 bits. Released chunks will be connected into linked list and grouped by size in coresponding [bins](#bins). Malloc will try to allocate a new chunk from previously freed chunks or free memory, if fail it will try to merge adjacent free chunks before calling kernel panic.

# _<kernel/klib/\_internal\_kmalloc.h>_
Table of contents:   

0. [Used data structures](#data-structures)   
1. [Allocating memory](#kmalloc)   
2. [Freeing memory](#free)   

## Data structures
### chunk_header
```c
struct chunk_header {
#ifdef i386
	size_t size 	 : 30;
#endif
	size_t free	     : 1; // Set when chunk was released
	size_t prev_free : 1; // Set when previous chunk was released
	size_t prev_size;     // Used when prev_free is set

	struct chunk_header *prev, *next;
};
typedef struct chunk_header* chunkptr_t;
```

### bin_info
```c
struct bin_info {
	size_t stored_size; // Indicates size of stored chunks
	size_t counter;
	chunkptr_t first;
	chunkptr_t last;
};
typedef struct bin_info bin_t;
```

### malloc_state
```c
struct malloc_state {
	int is_initialized;
	bin_t bins[BINS_QUANTITY];
	chunkptr_t void_chunk;
};
```

# kmalloc
## Chunk memory details
   - Minimal chunk size should be enought to hold whole [chunk header](#chunk_header).
   - Chunk size will be checked if it is sufficient to hold all data with required header informations.
   - User data starts after first two header fields (size, prev_size)
   
## Void chunk
Void chunk is created during malloc initialization and indicates free memory block used for creating new chunks.
   
## Algorithm
_All sub functions have to get already aligned size!_   

- Init [malloc_state struct](#malloc_state) on first run  
- Align size  
- Check if size is sufficient to hold all data  
- If not:  
    - Add aligement value to size  
 
- [Find released chunk](#searching-for-free-chunk)  
- If no freed chunk found:  
    - If enough free memory:  
        - Create new chunk  
        - Return chunk address  
    
    - Else:  
        - [Merge adjacent chunks](#merging-adjacent-chunks)  
        - [Find released chunk](#searching-for-free-chunk)  
        - Panic if no chunks found
       
- Create new chunk  
- Return chunk address  
          
### Searching for free chunk

- Find suitable or larger released chunk
- If found suitable:
    - Remove chunk from bin
    - Update adjacent chunk header
    - Update chunk header
    - Return chunk address
        
- If found larger chunk:
    - Remove chunk from bin
    - [Split chunk](#splitting-larger-chunk)
    - Return chunk address
- Return NULL if not found

### Splitting larger chunk
- Check if chunk can be splitted
- If yes:
    - Create smaller new chunk from old one
    - Create chunk from the rest of old chunk size
    - [Free](#free) the rest of old chunk
    - Return new chunk address
- Else:
    - Return chunk address

# kfree
## Bins
Size bins operate on the LIFO principle
   - Sizebin - single linked lists of chunks grouped by their size
       - Normal sizebin - store general purpose chunks with sizes 32, 48 … 96
       - Taskbin - store chunks with size equal TCB
   - Sortedbin - double linked list of diffrent size chunks sorted in increasing order

## Algorithm
- Exit if trying to free NULL ptr
- Throw error if free is used before malloc initialization
- Throw error if chunk was already freed (double free prevention)
- Find suitable bin  
- If found:
    - add chunk to the top of found bin
- If not found: 
    - insert chunk to Sortedbin
- Update bottom adjacent chunk header
- Set prev_free flag and prev_size field
- Mark chunk as free
