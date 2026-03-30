#include "kheap.h"
#include "memory_sensei.h"

KernelHeap k_heap = {0};

static inline u8 bitmap_get(u64 page) {
    return (k_heap.bitmap[page/8] >> (page%8)) & 1;
}
static inline void bitmap_set(u64 page) {
    k_heap.bitmap[page/8] |= (1 << (page%8));
}
static inline void bitmap_clear(u64 page) {
    k_heap.bitmap[page/8] &= ~(1 << (page%8));
}

static int _find_free_pages(u64 num_pages) {
    u64 count = 0;
    u64 start = 0;

    for (u64 x = k_heap.first_free_index; x < k_heap.index; x++) {
        if (!bitmap_get(x)) {
            if (count == 0)
                start = x;

            count++;

            if (count == num_pages)
                return start;
        } else {
            count = 0;
        }
    }

    return -1;
}

void start_kheap(MemorySensei* mem_sensei) {
    k_heap.base      = (u8*)KERNEL_HEAP_START;     // virtual memory address
    k_heap.capacity  = KERNEL_HEAP_NUM_PAGES;
    k_heap.page_size = KB(4);
    k_heap.index     = 0;

    for (u32 x = 0; x < k_heap.capacity / 8; x++) {
        k_heap.bitmap[x] = 0;
    }
}

// returns NULL on failure
void* kheap_reserve(u64 num_pages) {
    u64 abs_idx = k_heap.index * k_heap.page_size;

    if (k_heap.index + num_pages <= k_heap.capacity) {
        void* alloced_space = (void *)&k_heap.base[abs_idx];

        for(u64 x = 0; x < num_pages; x++)
            bitmap_set(k_heap.index+x);

        k_heap.index += num_pages;
        get_mem_sensei()->kernel_info.heap_bytes_used += (num_pages * k_heap.page_size);
        get_mem_sensei()->kernel_info.heap_bytes_free -= (num_pages * k_heap.page_size);
        return alloced_space;
    }

    int start = _find_free_pages(num_pages);
    if (start < 0)
        return NULL;

    for (u64 x = 0; x < num_pages; x++)
        bitmap_set(start + x);

    get_mem_sensei()->kernel_info.heap_bytes_used += (num_pages * k_heap.page_size);
    get_mem_sensei()->kernel_info.heap_bytes_free -= (num_pages * k_heap.page_size);

    return (void *)((u8 *)k_heap.base + (start * k_heap.page_size));
};

u32 kheap_free(void* ptr, u64 num_pages) {
    if (!ptr)
        return 0;
    if ((u64)ptr >= (k_heap.capacity * k_heap.page_size + (u64)k_heap.base) ||
            (u64)ptr < (u64)k_heap.base)
        return 0;

    u64 abs_idx = ((u64)ptr - (u64)k_heap.base) / k_heap.page_size;

    if (abs_idx < k_heap.first_free_index)
        k_heap.first_free_index = abs_idx;

    u64 freed = 0;

    for (u64 x = 0; x < num_pages; x++)
        if (bitmap_get(abs_idx+x)) {
            bitmap_clear(abs_idx+x);
            freed++;
        } else {
            HANG_PAGE(get_mem_sensei(), 
                        (u64)k_heap.base +((abs_idx+x)*k_heap.page_size), 
                        (u64)ptr, 
                        num_pages);
            u8* page = (u8*)k_heap.base +((abs_idx+x)*k_heap.page_size);
            for (u64 c = 0; c < k_heap.page_size; c++) {
                *page++ = KHFLAG_DFREE;             // poison page with 0xDEDEDE.... on double free
            }
            continue;
        }

    get_mem_sensei()->kernel_info.heap_bytes_used -= (freed * k_heap.page_size);
    get_mem_sensei()->kernel_info.heap_bytes_free += (freed * k_heap.page_size);
    return 1;
}


