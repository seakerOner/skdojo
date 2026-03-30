#ifndef KHEAP_H
#define KHEAP_H

#include "memory_sensei.h"

typedef struct {
    u8*   base;
    u64   index;
    u64   page_size;
    u64   capacity;

    u64   first_free_index;

    u8    bitmap[KERNEL_HEAP_NUM_PAGES / 8];    // free or not
} KernelHeap;

#define CEIL_PAGES(bytes, pagesizebytes)                           \
        ((bytes / pagesizebytes) + (bytes % pagesizebytes > 0))    \

void start_kheap(MemorySensei* mem_sensei);

// returns NULL on failure
void* kheap_reserve(u64 num_pages);

// on page double free poisons memory (0xDEDEDEDE....)
u32   kheap_free   (void* ptr, u64 num_pages);

#define KHFLAG_DFREE 0xDE

#endif
