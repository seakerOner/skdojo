#include "memory_sensei.h"

MemorySensei memory_sensei = {0};

PtTable heap_pts[PT_TABLES_FOR_KHEAP] __attribute__((aligned(PAGE_SIZE))) = {0};

MemorySensei* create_memory_sensei(BiosBootInfo* boot_info) {
    memory_sensei.internal.pd_index          = 1; // first pd entry is already filled by the bootloader
    memory_sensei.internal.kpage_max         = KERNEL_HEAP_NUM_PAGES;
    memory_sensei.internal.kpage_index       = 0;
    memory_sensei.kernel_info.heap_bytes_cap = KERNEL_HEAP_LEN;
                               
    for(u64 x = 0; x < boot_info->boot_memmap_entries; x++) {

        if (boot_info->boot_memmap_addr[x].type == USABLE_RAM
                && boot_info->boot_memmap_addr[x].base_addr >= CONFIG_PHYSICAL_START) {

            memory_sensei.physical_stats.bytes_usable += boot_info->boot_memmap_addr[x].length;


        } else if (boot_info->boot_memmap_addr[x].type == USABLE_RAM
                && boot_info->boot_memmap_addr[x].base_addr < CONFIG_PHYSICAL_START
                && boot_info->boot_memmap_addr[x].base_addr + boot_info->boot_memmap_addr[x].length 
                > CONFIG_PHYSICAL_START) {
            // if a memory section starts below the threshold but has available 
            // memory above it we take it
            memory_sensei.physical_stats.bytes_usable += (boot_info->boot_memmap_addr[x].length - 
                        (CONFIG_PHYSICAL_START - boot_info->boot_memmap_addr[x].base_addr));
            boot_info->boot_memmap_addr[x].base_addr = CONFIG_PHYSICAL_START;
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

    InternalMemSensei* memsensei = &memory_sensei.internal;
    // store usable ram in pages for the kernel heap
    for(u64 x = 0; x < boot_info->boot_memmap_entries; x++) {
        if (boot_info->boot_memmap_addr[x].type != USABLE_RAM)
            continue;

        u64 phy_addr    = boot_info->boot_memmap_addr[x].base_addr;
        u64   phy_len   = boot_info->boot_memmap_addr[x].length;

        u64 aligned_addr = phy_addr & ~4095;    // align to 4kb
        u64 aligned_len  = phy_len - (aligned_addr - phy_addr);
        u64 num_v_pages = aligned_len / PAGE_SIZE;

        for (u64 v = 0; v < num_v_pages; v++) {
            if (memsensei->kpage_index >= memsensei->kpage_max) 
                break;
            memsensei->kpages[memsensei->kpage_index++] = (u64)aligned_addr;
            aligned_addr += PAGE_SIZE;     // next 4kb
        }
    }
    u64 free_kernel_memory = 0;

    // map physical memory on pd_tables (skdojo by default makes tables for 14MB)
    for (u64 x = 0; x < memsensei->kpage_index; x++) {

        u64 phys = memsensei->kpages[x];
        u64 virt = KERNEL_HEAP_START + (x * PAGE_SIZE);
        free_kernel_memory += PAGE_SIZE;

        u64 pt_page_i   = (virt - KERNEL_HEAP_START) / PAGE_SIZE;

        u64 pt_number     = pt_page_i / PAGE_ENTRIES;
        u64 pt_entry_i    = pt_page_i % PAGE_ENTRIES;

        heap_pts[pt_number].entries[pt_entry_i] = 
            phys | PAGE_PRESENT | PAGE_WRITABLE; 
                             
        // if (!(boot_info->pml4_table->entries[pml4_i] & 1)) {
        //     // TODO: alloc new pdpt
        // }
        // if (!(boot_info->pdpt_table->entries[pdpt_i] & 1)) {
        //     // TODO: alloc new pd
        // }
        // if (!(boot_info->pd_table->entries[pd_i] & 1)) {
        //     // TODO: alloc new pt
        // }

    }
    // add pages to the pd table
    u64 used_pts = (memsensei->kpage_index + PAGE_ENTRIES - 1) / PAGE_ENTRIES;
    for (u64 x = 0; x < used_pts; x++) {
        u64 virt = KERNEL_HEAP_START + (x * PAGE_SIZE * PAGE_ENTRIES);
        u64 pd_i = PD_INDEX(virt);
        boot_info->pd_table->entries[pd_i] = 
            ((u64)&heap_pts[x]) | PAGE_PRESENT | PAGE_WRITABLE;   
        memsensei->pd_index++;
    }
    memory_sensei.kernel_info.heap_bytes_free = free_kernel_memory;
                               
    // reload CR3 register to maintain TBL (Translation Lookaside Buffer) 
    // coherence (in x86 architecture TLB coherence is not guaranteed when modifying table entries)
    //
    // The Translation Lookaside Buffer (TLB) is a specialized cache within the Memory Management Unit (MMU)
    // of x86 processors that stores recent virtual-to-physical address translations to avoid the performance 
    // penalty of walking page tables in main memory.
    __asm__ __volatile__ ("mov %%cr3, %%rax; mov %%rax, %%cr3" ::: "rax", "memory");
    return &memory_sensei;
}

MemorySensei* get_mem_sensei() {
    return &memory_sensei;
}
