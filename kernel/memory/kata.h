#ifndef KATA_H
#define KATA_H

// Kata: A physical memory allocator inspired by the Buddy System used by Linux

#include "../inttype.h"
#define MAX_ORDER 25

#define POW2(x) (1 << x)

typedef struct KataBlock {
    struct KataBlock* next;
} KataBlock;

typedef struct {
    KataBlock* free_lists[MAX_ORDER];
    u64 max_orders;
} KataAllocator;

KataAllocator* kata_init();

void kata_add_region(u64 addr, u64 size);

void kata_add_block(KataAllocator* ka, u64 addr, u32 order);

#endif
