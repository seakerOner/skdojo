#ifndef KATA_H
#define KATA_H

// Kata: A physical memory allocator inspired by the Buddy System used by Linux

#include "../inttype.h"
#include "../bios_boot_info.h"
#define MAX_ORDER 25

#define POW2(x) (1 << x)

typedef struct KataBlock {
    struct KataBlock* next;
    u32 order;
} KataBlock;

typedef struct {
    KataBlock* free_lists[MAX_ORDER];
    u64 max_orders;
} KataAllocator;

KataAllocator* kata_init();

void* kata_alloc( u64 order );
void kata_free( u64 addr );

void kata_populate_regions( BiosBootInfo* bios_boot_info );

void kata_add_region( u64 addr, u64 size );
void kata_add_block( u64 addr, u32 order );

#endif
