#include "kheap.h"
#include "kata.h"
#include "memory_sensei.h"
#include "../printk/printk.h"

static KernelHeap k_heap = {0};

static inline u8 bitmap_get( u64 page ) {
    return ( k_heap.bitmap[page/8] >> ( page % 8 ) ) & 1;
}
static inline void bitmap_set( u64 page ) {
    k_heap.bitmap[page/8] |= ( 1 << ( page % 8 ) );
}
static inline void bitmap_clear( u64 page ) {
    k_heap.bitmap[page/8] &= ~( 1 << ( page % 8 ) );
}

static int _find_free_pages( u64 num_pages ) {
    u64 count = 0;
    u64 start = 0;

    for ( u64 x = k_heap.first_free_index; x < k_heap.capacity; x++ ) {
        if ( !bitmap_get( x ) ) {
            if ( count == 0 )
                start = x;

            count++;

            if ( count == num_pages )
                return start;
        } else {
            count = 0;
        }
    }

    return -1;
}

static int _find_realloc_pages_free( u64 ptr_idx, u64 old_num_pages, u64 num_pages ) {
    i64 extra_pages = num_pages - old_num_pages;
    if ( extra_pages < 0 ) return -1;

    u64 count = 0;

    for ( u64 x = ptr_idx+num_pages; x < k_heap.capacity; x++ ) {
        if ( !bitmap_get( x ) ) {
            count++;

            // there is enough space to simply increase the size without base reallocation
            if ( count == ( u64 ) extra_pages )
                return ptr_idx;    
        } else {
            if ( count < ( u64 ) extra_pages)
                break;
        }
    }

    // we need to search for available space, realloc the memory and mark as free the old memory space
    u64 start = 0;
    count     = 0;

    for ( u64 x = k_heap.first_free_index; x < k_heap.capacity; x++ ) {
        if ( !bitmap_get( x ) ) {
            if ( count == 0 )
                start = x;

            count++;

            if ( count == num_pages ) {
                // mark as free the old pages
                for ( u64 p = ptr_idx; p < ptr_idx + old_num_pages; p++ )
                    if ( bitmap_get( p ) )
                        bitmap_clear( p );

                // mark as used the new already reserved pages
                for ( u64 p = start; p < start + old_num_pages; p++ )
                    if ( !bitmap_get( p ) )
                        bitmap_set( p );

                // copy bytes over to new location
                u8* old_ptr   = k_heap.base + ( ptr_idx * k_heap.page_size );
                u8* new_ptr   = k_heap.base + ( start * k_heap.page_size );
                u64 num_bytes = k_heap.page_size * old_num_pages;

                for ( u64 b = 0; b < num_bytes; b++ )
                     new_ptr[b] = old_ptr[b];

                return start;
            }
        } else {
            count = 0;
        }
    }

    return -1;
}

static void map_kheap() {
    u64* free_mem = &get_mem_sensei()->kernel_info.heap_bytes_free;

    u64 virt = ( u64 )k_heap.base;
    u64 cap  = ( k_heap.capacity * KB( 4 ) ) / MB( 2 );

    for (u64 x = 0; x < cap; x++) {
        void* ptr = kata_alloc( ORDER_2MB, TRUE );
        if ( ptr == NULL ) {
            // TODO:
            printk_err( KSTR( "[KHEAP][map_kheap] `kata_alloc` failed to allocate a 2MB ORDER" ));
            continue;
        }
        u64 phys  = ( u64 )VA_TO_PA( ptr );
        map_2MB_page(virt, phys);
        virt      += MB( 2 );
        *free_mem += MB(2);
    }
};

void start_kheap( MemorySensei* mem_sensei ) {
    // virtual memory address
    k_heap.base             = ( u8* )( KERNEL_HEAP_START+HIGH_MEM_IDENTITY );     
    k_heap.page_size        = PAGE_SIZE;
    k_heap.capacity         = KERNEL_HEAP_NUM_PAGES;
    k_heap.index            = 0;
    k_heap.first_free_index = 0;

    for ( u32 x = 0; x < k_heap.capacity / 8; x++ ) {
        k_heap.bitmap[x] = 0;
    }

    map_kheap();
}

// returns NULL on failure
void* kheap_reserve( u64 num_pages ) {
    int start = _find_free_pages( num_pages );
    if ( start < 0 )
        return NULL;


    for ( u64 x = 0; x < num_pages; x++ )
        bitmap_set( start + x );

    k_heap.index += num_pages;
    get_mem_sensei()->kernel_info.heap_bytes_used += ( num_pages * k_heap.page_size );
    get_mem_sensei()->kernel_info.heap_bytes_free -= ( num_pages * k_heap.page_size );

    return ( void * )( ( u8 * )( k_heap.base + ( start * k_heap.page_size) ) );
};

void* kheap_resize( void* ptr, u64 old_num_pages, u64 num_pages ) {
    if ( !ptr )
        return NULL;
    if (( u64 )ptr >= ( k_heap.capacity * k_heap.page_size + ( u64 )k_heap.base ) ||
            ( u64 )ptr < ( u64 )k_heap.base)
        return NULL;

    u64 abs_idx = ( ( u64 )ptr - ( u64 )k_heap.base ) / k_heap.page_size;

    int res = _find_realloc_pages_free( abs_idx, old_num_pages, num_pages );

    if ( res < 0 )
        return NULL;

    for ( u64 x = 0; x < ( u64 )num_pages; x++ )
        bitmap_set( res + x );

    get_mem_sensei()->kernel_info.heap_bytes_used += (( num_pages * k_heap.page_size ) 
                                                        - ( old_num_pages * k_heap.page_size ));
    get_mem_sensei()->kernel_info.heap_bytes_free -= (( num_pages * k_heap.page_size )
                                                        - ( old_num_pages * k_heap.page_size ));

    return (void *)(( u8* )( k_heap.base + ( res * k_heap.page_size )));
};

u32 kheap_free( void* ptr, u64 num_pages ) {
    if ( !ptr )
        return 0;
    if (( u64 )ptr >= ( k_heap.capacity * k_heap.page_size + ( u64 )k_heap.base ) ||
            ( u64 )ptr < ( u64 )k_heap.base)
        return 0;

    u64 abs_idx = (( ( u64 )ptr ) - ( u64 )k_heap.base ) / k_heap.page_size;

    if ( abs_idx < k_heap.first_free_index )
        k_heap.first_free_index = abs_idx;

    u64 freed = 0;

    for ( u64 x = 0; x < num_pages; x++ )
        if ( bitmap_get( abs_idx+x ) ) {
            bitmap_clear( abs_idx+x );
            freed++;
        } else {
            HANG_PAGE( get_mem_sensei(), 
                        ( u64 )k_heap.base +( ( abs_idx+x ) * k_heap.page_size ), 
                        ( u64 )ptr, 
                        num_pages );
            u8* page = ( u8* )k_heap.base +( ( abs_idx+x ) *k_heap.page_size );
            for ( u64 c = 0; c < k_heap.page_size; c++ ) {
                *page++ = KHFLAG_DFREE;             // poison page with 0xDEDEDE.... on double free
            }
            continue;
        }

    get_mem_sensei()->kernel_info.heap_bytes_used -= ( freed * k_heap.page_size );
    get_mem_sensei()->kernel_info.heap_bytes_free += ( freed * k_heap.page_size );
    return 1;
}


