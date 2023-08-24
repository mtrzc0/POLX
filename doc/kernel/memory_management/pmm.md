# ***Just manage phisical memory***
The physical memory manager divides the available physical memory (RAM) into fixed-length blocks called frames and handles their allocation and release.   

## _<kernel/pmm.h>_
Table of contents:    

0. [Global data](#global-data)   
1. [Initialization](#initialization)   
2. [Frame allocation](#allocation)   
3. [Frame release](#release)   


## **Global data**
```c
typedef struct {
        uint32_t total_frames;
        uint32_t free_frames;
} PMM_INFO;
```
Globally avaliable data share only information of used physical memory.   

## **Initialization**
```c
void pmm_init(uintptr_t total_memory, uintptr_t reserved_start, 
                                          uintptr_t reserved_end);
```
Create a structure that stores information about free and allocated physical memory. The data needed for initialization is provided by the bootloader and the labels contained in the linker script.   

## **Allocation**
```c
uintptr_t pmm_get_frame(void);
```
Find free frame and return its address.   

```c
uintptr_t pmm_get_pool(size_t frames);
```
Find physical contiguous memory space and return starting address

## **Release**
```c
void pmm_free_frame(uintptr_t paddr);
```
Just mark frame as free.   

```c
void pmm_free_pool(uintptr_t start_of_pool, size_t frames);
```
Mark all frames from pool as free