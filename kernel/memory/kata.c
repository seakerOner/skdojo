#include "memory_sensei.h"
#include "kata.h"

static KataAllocator kata_alloc = {0};

KataAllocator* kata_init() {
    kata_alloc.max_orders = MAX_ORDER;
    return &kata_alloc;
} 


void kata_populate_regions(BiosBootInfo* bios_boot_info) {
}

void kata_add_region(u64 addr, u64 size) {
    while (size > 0) {
        u32 order = MAX_ORDER;

        while (order > 0) {
            u64 block_size = PAGE_SIZE << order;

            if (addr % block_size == 0 && size >= block_size)
                break;

            order--;
        }

        kata_add_block(&kata_alloc, addr, order);

        addr += (PAGE_SIZE << order);
        size -= (PAGE_SIZE << order);
    }
}

void kata_add_block(KataAllocator* ka, u64 addr, u32 order) {
    KataBlock* block = (KataBlock*)PA_TO_VA(addr);

    block->next = ka->free_lists[order];
    ka->free_lists[order] = block;
}
