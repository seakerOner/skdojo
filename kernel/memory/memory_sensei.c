#include "memory_sensei.h"
#include "kata.h"
#include "kheap.h"

static MemorySensei memory_sensei __attribute__(( aligned( PAGE_SIZE ), used )) = {0};

BootstrapLayout* get_bootstrap() {
    volatile u8* ptr;
    ptr = ( volatile u8* )MB( 1 ); // bootstrap_start
    return ( BootstrapLayout* )ptr;
}

static inline void set_tables_physmap() {
    BootstrapLayout* bootstrap = get_bootstrap();
    RECURSIVE_PML4->entries[PML4_INDEX(PHYSMAP_BASE)] 
        = (u64)&bootstrap->physmap_pdpt | PAGE_PRESENT | PAGE_WRITABLE;

    for (u32 x = 0; x < 128; x++) 
        RECURSIVE_PDPT( PML4_INDEX( PHYSMAP_BASE ) )->entries[x] = 
            (u64)&bootstrap->physmap_pds[x] | PAGE_PRESENT | PAGE_WRITABLE;
}

void map_4KB_page( u64 virt, u64 phys ) {
    RECURSIVE_PT(
            PML4_INDEX(virt), 
            PDPT_INDEX(virt), 
            PD_INDEX(virt) )->entries[PT_INDEX( virt )] =  
        phys | PAGE_PRESENT | PAGE_WRITABLE;

    invlpg( virt );
}

void map_2MB_page( u64 virt, u64 phys ) {
    RECURSIVE_PD( PML4_INDEX( virt ), PDPT_INDEX( virt ) )
        ->entries[PD_INDEX( virt )] =
            phys | PAGE_PRESENT | PAGE_WRITABLE | PAGE_PS;      // PAGE_PS = 2MB pages

    invlpg( virt );
}

MemorySensei* create_memory_sensei( BiosBootInfo* boot_info ) {
    KataAllocator* ka = kata_init();

    memory_sensei.kata = ka;

    memory_sensei.kernel_info.heap_bytes_cap  = KERNEL_HEAP_LEN;
    memory_sensei.kernel_info.heap_page_max   = KERNEL_HEAP_NUM_PAGES;
    memory_sensei.kernel_info.heap_bytes_free = 0;
                               
    for ( u64 x = 0; x < boot_info->boot_memmap_entries; x++ ) {
        switch ( boot_info->boot_memmap_addr[x].type ) {
            case USABLE_RAM:
                if ( boot_info->boot_memmap_addr[x].base_addr >= CONFIG_PHYSICAL_START ) {
                    memory_sensei.physical_stats.bytes_usable += boot_info->boot_memmap_addr[x].length;

                } else if ( boot_info->boot_memmap_addr[x].base_addr < CONFIG_PHYSICAL_START
                && boot_info->boot_memmap_addr[x].base_addr + boot_info->boot_memmap_addr[x].length 
                > CONFIG_PHYSICAL_START ) {

                    // if a memory section starts below the threshold but has available 
                    // memory above it we take it
                    memory_sensei.physical_stats.bytes_usable += ( boot_info->boot_memmap_addr[x].length - 
                            ( CONFIG_PHYSICAL_START - boot_info->boot_memmap_addr[x].base_addr ) );
                    u64 old_addr = boot_info->boot_memmap_addr[x].base_addr;
                    boot_info->boot_memmap_addr[x].base_addr = CONFIG_PHYSICAL_START;
                    boot_info->boot_memmap_addr[x].length    = 
                        ( boot_info->boot_memmap_addr[x].length - (boot_info->boot_memmap_addr[x].base_addr - old_addr ) );
                }
                break;
            case BAD_MEMORY:
                memory_sensei.physical_stats.bytes_bad_mem += boot_info->boot_memmap_addr[x].length;
                break;
            case RESERVED:
            case ACPI_NVS:
            case ACPI_RECLAIM:
                memory_sensei.physical_stats.bytes_reserved += boot_info->boot_memmap_addr[x].length;
                break;
            default:
                // huh?
                break;
        }
    }

    // set recursive paging
    boot_info->pml4_table->entries[PML4_RECURSIVE_SLOT] = 
        ( u64 )boot_info->pml4_table | PAGE_PRESENT | PAGE_WRITABLE;
    reload_cr3();

    set_tables_physmap();
    reload_cr3();

    for( u64 x = 0; x < boot_info->boot_memmap_entries; x++ ) {
        if ( boot_info->boot_memmap_addr[x].type != USABLE_RAM )
            continue;

        u64 phy_addr     = boot_info->boot_memmap_addr[x].base_addr;
        u64 phy_len      = boot_info->boot_memmap_addr[x].length;

        u64 aligned_addr = ALIGN_UP( phy_addr, KB( 4 ) );
        u64 aligned_len  = ALIGN_DOWN( phy_len, KB( 4 ) );
        u64 end          = aligned_addr + aligned_len;

        if ( end <= aligned_addr )
            continue;

        for ( u64 addr = aligned_addr; addr < end; addr += MB( 2 ) ) {
            u64 phys = addr;
            u64 virt = PHYSMAP_BASE + phys;
            map_2MB_page( virt, phys );
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
