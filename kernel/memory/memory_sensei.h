#ifndef MEMORY_SENSEI_H
#define MEMORY_SENSEI_H

#include "../inttype.h"
#include "../bios_boot_info.h"

#define CONFIG_PHYSICAL_START 0x100000

#define STAGE2BOOT_PHY_ADDR 0x10000
#define KERNEL_PHY_ADDR     0x20000
#define BOOT_INFO_ADDR      0x50000
#define CPU_STACK           0x90000

#define PAGE_SIZE   (4 * 1024)
#define PAGE_ENTRIES 512        

//#define KERNEL_HEAP_START 0x200000                 // 2MB (virtual address)
#define KERNEL_HEAP_START   0x200000                 // 2MB (virtual address)
#define KERNEL_HEAP_LEN  (0x01000000 - 0x00200000)   // bytes (14MB)

#define KERNEL_HEAP_NUM_PAGES (KERNEL_HEAP_LEN / PAGE_SIZE)
#define PT_TABLES_FOR_KHEAP ((KERNEL_HEAP_NUM_PAGES + PAGE_ENTRIES - 1) / PAGE_ENTRIES)

#define PML4_INDEX(v_addr) ((v_addr >> 39) & 0x1FF)
#define PDPT_INDEX(v_addr) ((v_addr >> 30) & 0x1FF)
#define PD_INDEX(v_addr)   ((v_addr >> 21) & 0x1FF)
#define PT_INDEX(v_addr)   ((v_addr >> 12) & 0x1FF)

//
// 0x00000000 - 0x00200000   -> identity (boot, kernel) 2MB
// 0x00200000 - 0x01000000   -> kernel heap
// 0x01000000 - ...          -> future uses...
//

#define PAGE_PRESENT         (1ULL << 0)
#define PAGE_WRITABLE        (1ULL << 1)
#define PAGE_USER_SUPERVISOR (1ULL << 2)
#define PAGE_WRITE_THROUGH   (1ULL << 3)
#define PAGE_CACHE_DISABLE   (1ULL << 4)
#define PAGE_ACCESSED        (1ULL << 5)
#define PAGE_AVAILABLE       (1ULL << 6)
#define PAGE_GLOBAL          (1ULL << 8)


typedef struct {
   u64 bytes_usable;
   u64 bytes_reserved;
   u64 bytes_bad_mem;
} MemoryPhyInfo;

typedef struct {
    u64 page_address;
    struct {
        u64 original_freed_address;
        u64 pages_tried_tofree;
    } backtrace;
} FailedPages;

typedef struct {
   u64 heap_bytes_cap;
   u64 heap_bytes_used;
   u64 heap_bytes_free;

   u64 heap_pages_hanged;
   FailedPages hanged_pages[KERNEL_HEAP_NUM_PAGES];
} MemoryKernelInfo;

// used when trying to double free a page in kernel heap
#define HANG_PAGE(mem_sensei, hang_page_addr, caller_addr, tried_num_pages)                             \
    do {                                                                                                \
        u64 idx = mem_sensei->kernel_info.heap_pages_hanged++;                                          \
        mem_sensei->kernel_info.hanged_pages[idx].page_address                     = hang_page_addr;    \
        mem_sensei->kernel_info.hanged_pages[idx].backtrace.original_freed_address = caller_addr;       \
        mem_sensei->kernel_info.hanged_pages[idx].backtrace.pages_tried_tofree     = tried_num_pages;   \
    } while (0);                                                                                        \

typedef struct {
    u64 pd_index;
    u64 kpages[KERNEL_HEAP_NUM_PAGES];
    u64 kpage_index;
    u64 kpage_max;
} InternalMemSensei;

typedef struct {
    MemoryPhyInfo physical_stats;
    MemoryKernelInfo kernel_info;
    InternalMemSensei internal;
}  MemorySensei;

MemorySensei* create_memory_sensei(BiosBootInfo* boot_info);
MemorySensei* get_mem_sensei();

#endif
