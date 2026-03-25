#ifndef MEMORY_SENSEI_H
#define MEMORY_SENSEI_H

#include "../inttype.h"
#include "../bios_boot_info.h"

#define CONFIG_PHYSICAL_START 0x100000

#define STAGE2BOOT_PHY_ADDR 0x10000
#define KERNEL_PHY_ADDR     0x20000
#define BOOT_INFO_ADDR      0x50000
#define CPU_STACK           0x90000

#define KERNEL_HEAP_LEN  (0x01000000 - 0x00200000)   // bytes (14MB)

//
// 0x00000000 - 0x00200000   -> identity (boot, kernel) 2MB
// 0x00200000 - 0x01000000   -> kernel heap
// 0x01000000 - ...          -> future uses...

typedef struct {
   u64 bytes_usable;
   u64 bytes_reserved;
   u64 bytes_bad_mem;
} MemoryPhyInfo;

typedef struct {
   u64 heap_bytes_cap;
   u64 heap_bytes_used;
   u64 heap_bytes_free;
} MemoryKernelInfo;

typedef struct {
    MemoryPhyInfo physical_stats;
    MemoryKernelInfo kernel_info;
    u64 pd_index;
}  MemorySensei;

MemorySensei* create_memory_sensei(BiosBootInfo* boot_info);

#endif
