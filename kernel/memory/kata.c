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

void kata_add_block( u64 addr, u32 order ) {
    KataBlock* block = ( KataBlock* ) PA_TO_VA( addr );
    KataAllocator* ka = &kata_allocator;

    block->next = ka->free_lists[order];
    ka->free_lists[order] = block;
}

void* kata_alloc( u64 order ) {
    KataAllocator* ka = &kata_allocator;

    for ( u32 x = order; x <= ka->max_orders; x++ ) {
        if ( ka->free_lists[x] == NULL )
            continue;

        KataBlock* blk = ka->free_lists[x];
        ka->free_lists[x] = blk->next;

        // if higher order than request, split memory block until we get a block of the desired ORDER
        while ( x > order ) {
            x--;

            u64 block_addr     = ( u64 )VA_TO_PA( ( u64 )blk );
            u64 split_size     = KB( 4 ) << x ;

            u64 split_blk_addr = block_addr + split_size;

            kata_add_block( split_blk_addr, x );
        }

        u8* ptr = ( u8* ) blk;

        ptr += sizeof( KataBlock );     // skip metadata

        return ( void * )ptr;
    }

    return NULL;
}

void kata_free( u64 addr, u64 order ) {
}
