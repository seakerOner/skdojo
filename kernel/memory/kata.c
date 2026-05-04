#include "memory_sensei.h"
#include "kata.h"

static KataAllocator kata_allocator = {0};

KataAllocator* kata_init() {
    kata_allocator.max_orders = MAX_ORDER;
    FILL( kata_allocator.free_lists, 0, sizeof( kata_allocator.free_lists ));
    return &kata_allocator;
} 

void kata_populate_regions( BiosBootInfo* boot_info ) {
    for( u64 x = 0; x < boot_info->boot_memmap_entries; x++ ) {
        if ( boot_info->boot_memmap_addr[x].type != USABLE_RAM )
            continue;

        u64 phy_addr     = boot_info->boot_memmap_addr[x].base_addr;
        u64 phy_len      = boot_info->boot_memmap_addr[x].length;
        u64 aligned_addr = phy_addr;

        aligned_addr    = ALIGN_UP( aligned_addr, KB( 4 ) );
        u64 aligned_len = ALIGN_DOWN( phy_len, KB( 4 ) );
        u64 end         = aligned_addr + aligned_len;

        if ( end <= aligned_addr )
            continue;

        u64 lower_bound = ( u64 ) PA_TO_VA( aligned_addr );
        if ( !IS_PHYSMAP_VALID( lower_bound ) )
            continue;

        kata_add_region( aligned_addr, aligned_len );
    }
}

// addr must be PA
void kata_add_region( u64 addr, u64 size ) {
    while ( size > 0 ) {
        u64 order = MAX_ORDER;

        while ( order > 0 ) {
            u64 block_size =  KB( 4 ) << order;

            if ( block_size && addr % block_size == 0 && size >= block_size )
                break;

            order--;
        }

        kata_add_block( addr, order );

        addr += ( KB( 4 ) << order );
        size -= ( KB( 4 ) << order );
    }
}

// addr must be PA
void kata_add_block( u64 addr, u32 order ) {
    KataBlock* block        = ( KataBlock* ) PA_TO_VA( addr );
    KataAllocator* ka       = &kata_allocator;

    block->next             = ka->free_lists[order];
    block->order            = order;
    ka->free_lists[order]   = block;
}

// returns VA address
void* kata_alloc( u64 order, boolean raw_mode ) {
    KataAllocator* ka = &kata_allocator;

    for ( u32 x = order; x <= ka->max_orders; x++ ) {
        if ( ka->free_lists[x] == NULL )
            continue;

        KataBlock* blk = ka->free_lists[x];
        ka->free_lists[x] = blk->next;

        // if higher order than requested, split memory block until we get a block of the desired ORDER
        while ( x > order ) {
            x--;

            u64 block_addr     = ( u64 )VA_TO_PA( ( u64 )blk );
            u64 split_size     = KB( 4 ) << x ;

            u64 split_blk_addr = block_addr + split_size;

            kata_add_block( split_blk_addr, x );
        }

        u8* ptr = ( u8* ) blk;

        if (!raw_mode) 
            ptr += sizeof( KataBlock );     // skip metadata

        return ptr;
    }

    return NULL;
}

// addr must be VA
void kata_free( u64 addr ) {
    KataAllocator* ka   =  &kata_allocator;
    KataBlock* blk      = ( KataBlock* )( ( u8* )addr - sizeof( KataBlock ) );
    u64 p_addr          = ( u64 )VA_TO_PA( ( u64 )blk );
    u32 order           = blk->order;

    while ( order < ka->max_orders ) {
        u64 blk_size         = KB( 4 ) << order;

        u64 split_addr       = p_addr ^ blk_size;

        if ( !IS_PHYSMAP_VALID( ( u64 )PA_TO_VA( split_addr ) ) )
            break;

        KataBlock* prev      = NULL;
        KataBlock* curr      = ka->free_lists[order];

        while ( curr ) {
             /* 
              * if 'split_addr' is on a different order than requested then 
              * `split_addr` itself is split, making it not suitable to be merged
              * */ 
            if ( ( u64 )VA_TO_PA( ( u64 )curr ) == split_addr 
                    && curr->order == order)    
                break;

            prev = curr;
            curr = curr->next;
        }

        // not a free split
        if ( !curr )
            break;

        // remove split from free list
        if ( prev )
            prev->next = curr->next;
        else 
            ka->free_lists[order] = curr->next;

        if ( split_addr < p_addr )
            p_addr = split_addr;

        order++;
    }

    kata_add_block( p_addr, order );
}

// addr must be VA
void kata_free_raw( u64 addr, u32 order ) {
    // restore metadata
    KataBlock* blk  = ( KataBlock* )addr;
    FILL( blk, 0, sizeof( KataBlock ) );
    blk->order      = order;

    // `kata_free` expects `addr` to be after metadata
    addr = ( u64 )(( u8* )addr + sizeof( KataBlock ));

    kata_free( addr );
}
