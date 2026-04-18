#include "memory_sensei.h"
#include "kata.h"
#include "kheap.h"

static MemorySensei memory_sensei __attribute__((aligned(PAGE_SIZE), used)) = {0};

static inline BootstrapLayout* get_bootstrap() {
    volatile u8* ptr;
    ptr = (volatile u8*)MB(1); // bootstrap_start
    return (BootstrapLayout*)ptr;
}

u64 kheap_virt_to_phys(u64 virt) {
    if (virt < KERNEL_HEAP_START || virt > (KERNEL_HEAP_START + KERNEL_HEAP_LEN))
        return POISON;      // poison 

    u64 entry = RECURSIVE_PT(
            PML4_INDEX(virt), 
            PDPT_INDEX(virt), 
            PD_INDEX(virt)
        )->entries[PT_INDEX(virt)];

    return entry & ~0xFFF;
}

static inline void set_tables_physmap() {
    BootstrapLayout* bootstrap = get_bootstrap();
    RECURSIVE_PML4->entries[PML4_INDEX(PHYSMAP_BASE)] 
        = (u64)&bootstrap->physmap_pdpt | PAGE_PRESENT | PAGE_WRITABLE;

    for (u32 x = 0; x < 128; x++) 
        RECURSIVE_PDPT(PML4_INDEX(PHYSMAP_BASE))->entries[x] = 
            (u64)&bootstrap->physmap_pds[x] | PAGE_PRESENT | PAGE_WRITABLE;
}

static inline void _bootstrap_kheap(BiosBootInfo* boot_info, InternalMemSensei* memsensei) {
    BootstrapLayout* bootstrap = get_bootstrap();
    FILL(bootstrap, 0, sizeof(BootstrapLayout));
    u64 free_kernel_memory = 0;

    // map physical memory on pd_tables (skdojo by default makes tables for 14MB)
    for (u64 x = 0; x < memsensei->kpage_index; x++) {

        u64 phys = memsensei->kpages[x];
        u64 virt = KERNEL_HEAP_START + (x * PAGE_SIZE);

        free_kernel_memory += PAGE_SIZE;

        u64 pt_page_i     = (virt - KERNEL_HEAP_START) / PAGE_SIZE;

        u64 pt_number     = pt_page_i / PAGE_ENTRIES;
        u64 pt_entry_i    = pt_page_i % PAGE_ENTRIES;

        bootstrap->heap_pts[pt_number].entries[pt_entry_i] = 
            phys | PAGE_PRESENT | PAGE_WRITABLE; 
                             
    }

    // add pages to the pd table
    u64 used_pts = (memsensei->kpage_index + PAGE_ENTRIES - 1) / PAGE_ENTRIES;
    for (u64 x = 0; x < used_pts; x++) {
        u64 virt = KERNEL_HEAP_START + (x * PAGE_SIZE * PAGE_ENTRIES);
        u64 pd_i = PD_INDEX(virt);
        
        // since identity map and kernel are mirroed placing these tables on low memory indexes 
        // the kernel already has access to the kheap in high memory VA address space :D
        boot_info->pd_table->entries[pd_i] = ((u64)&bootstrap->heap_pts[x]) | PAGE_PRESENT | PAGE_WRITABLE; 
        
        memsensei->pd_index++;
    }
    memory_sensei.kernel_info.heap_bytes_free = free_kernel_memory;
    memory_sensei.kernel_info.heap_bytes_cap  = free_kernel_memory;
};

void map_2MB_page(u64 virt, u64 phys) {
    RECURSIVE_PD(PML4_INDEX(virt), PDPT_INDEX(virt))
        ->entries[PD_INDEX(virt)] =
            phys | PAGE_PRESENT | PAGE_WRITABLE | PAGE_PS;      // PAGE_PS = 2MB pages

    invlpg(virt);
}

MemorySensei* create_memory_sensei(BiosBootInfo* boot_info) {

    memory_sensei.kata                       = kata_init();
    memory_sensei.internal.kpage_index       = 0;
    memory_sensei.internal.pd_index          = 1; // first pd entry is already filled by the bootloader
    memory_sensei.internal.kpage_max         = KERNEL_HEAP_NUM_PAGES;
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

    // we will map 14MB with 4KB granuality on PML4 for the KHEAP
    // and for the PHYSMAP we use pages with 2MB granuality
    boolean kheap_bootstraped    = FALSE;
    InternalMemSensei* memsensei = &memory_sensei.internal;

    for(u64 x = 0; x < boot_info->boot_memmap_entries; x++) {
        if (boot_info->boot_memmap_addr[x].type != USABLE_RAM)
            continue;

        u64 phy_addr     = boot_info->boot_memmap_addr[x].base_addr;
        u64 phy_len      = boot_info->boot_memmap_addr[x].length;
        u64 aligned_addr = phy_addr;

        if (!kheap_bootstraped) {
            aligned_addr &= ~4095;    // align to 4kb
            if (aligned_addr < phy_addr)
                aligned_addr += PAGE_SIZE;

            u64 aligned_len  = phy_len - (aligned_addr - phy_addr);
            u64 num_v_pages  = aligned_len / PAGE_SIZE;

            for (u64 v = 0; v < num_v_pages; v++) {
                if (memsensei->kpage_index < memsensei->kpage_max)  {   // add to kheap
                    memsensei->kpages[memsensei->kpage_index++] = (u64)aligned_addr;
                } else {   

                    _bootstrap_kheap(boot_info, memsensei);
                    start_kheap(&memory_sensei);

                    // set recursive paging
                    boot_info->pml4_table->entries[PML4_RECURSIVE_SLOT] = 
                        (u64)boot_info->pml4_table | PAGE_PRESENT | PAGE_WRITABLE;
                    reload_cr3();

                    set_tables_physmap();
                    reload_cr3();

                    aligned_addr += PAGE_SIZE;     // next 4kb

                    kheap_bootstraped = TRUE;
                    break;
                    //goto _start_phys_map;          // start mapping from where kheap stopped
                }
                aligned_addr += PAGE_SIZE;     // next 4kb
            }
            continue;
        }

        _start_phys_map:    
        ;
        aligned_addr   &= ~(MB(2)-1);   // align to 2MB
        u64 aligned_len = phy_len - (aligned_addr - phy_addr);
        u64 end         = aligned_addr + aligned_len;

        for (u64 addr = aligned_addr; addr < end; addr += MB(2)) {
            u64 phys = addr;
            u64 virt = PHYSMAP_BASE + phys;
            map_2MB_page(virt, phys);
        }
    }
    boot_info->pml4_table->entries[0] = 0;        // remove identity map from pml4
    
    // reload CR3 register to maintain TLB (Translation Lookaside Buffer) 
    // coherence (in x86 architecture TLB coherence is not guaranteed when modifying table entries)
    //
    // The Translation Lookaside Buffer (TLB) is a specialized cache within the Memory Management Unit (MMU)
    // of x86 processors that stores recent virtual-to-physical address translations to avoid the performance 
    // penalty of walking page tables in main memory.
    reload_cr3();

    return &memory_sensei;
}

MemorySensei* get_mem_sensei() {
    return &memory_sensei;
}
