#include "memory_sensei.h"

MemorySensei memory_sensei;

MemorySensei* create_memory_sensei(BiosBootInfo* boot_info) {
    memory_sensei.pd_index = 1; // first pd entry is already filled by the bootloader
    memory_sensei.kernel_info.heap_bytes_cap = KERNEL_HEAP_LEN;
                               
    for(u64 x = 0; x < boot_info->boot_memmap_entries; x++) {

        if (boot_info->boot_memmap_addr[x].type == USABLE_RAM
                && boot_info->boot_memmap_addr[x].base_addr >= CONFIG_PHYSICAL_START) {

            memory_sensei.physical_stats.bytes_usable += boot_info->boot_memmap_addr[x].length;


        } else if (boot_info->boot_memmap_addr[x].type == USABLE_RAM
                && boot_info->boot_memmap_addr[x].base_addr < CONFIG_PHYSICAL_START
                && boot_info->boot_memmap_addr[x].length > CONFIG_PHYSICAL_START) {
            // if a memory section starts below the threshold but has available 
            // memory above it we take it
            memory_sensei.physical_stats.bytes_usable += (boot_info->boot_memmap_addr[x].length - 
                        (CONFIG_PHYSICAL_START - boot_info->boot_memmap_addr[x].base_addr));
        }

        if (boot_info->boot_memmap_addr[x].type == BAD_MEMORY) {
            memory_sensei.physical_stats.bytes_bad_mem += boot_info->boot_memmap_addr[x].length;
        }
        if (boot_info->boot_memmap_addr[x].type == RESERVED) {
            memory_sensei.physical_stats.bytes_reserved += boot_info->boot_memmap_addr[x].length;
        }
        if (boot_info->boot_memmap_addr[x].type == ACPI_NVS) {
            memory_sensei.physical_stats.bytes_reserved += boot_info->boot_memmap_addr[x].length;
        }
        if (boot_info->boot_memmap_addr[x].type == ACPI_RECLAIM) {
            memory_sensei.physical_stats.bytes_reserved += boot_info->boot_memmap_addr[x].length;
        }
    }
                               
    return &memory_sensei;
}
