#ifndef KATA_H
#define KATA_H

/*
 * Kata: A physical memory allocator inspired by the Buddy System used by Linux
 *
 * Allocation modes:
 *
 * - Normal mode:
 *   Returns a pointer after `KataBlock` metadata. Metadata is stored
 *   inside the block and used internally for free/merge operations.
 *
 * - Raw mode:
 *   Returns the full block (including metadata space). The caller owns
 *   the *entire* memory region and MUST provide the correct order when
 *   freeing via `kata_free_raw()`, as metadata may be overwritten.
 */

#include "../inttype.h"
#include "../bios_boot_info.h"
#define MAX_ORDER 25

#define POW2(x) (1 << x)

typedef struct KataBlock {
    struct KataBlock* next;
    u32 order;
} KataBlock;

typedef struct {
    KataBlock* free_lists[MAX_ORDER];
    u64 max_orders;
} KataAllocator;

#define ORDER_4KB    0
#define ORDER_8KB    1
#define ORDER_16KB   2
#define ORDER_32KB   3
#define ORDER_64KB   4
#define ORDER_128KB  5
#define ORDER_256KB  6
#define ORDER_512KB  7
#define ORDER_1MB    8
#define ORDER_2MB    9
#define ORDER_4MB    10
#define ORDER_8MB    11
#define ORDER_16MB   12
#define ORDER_32MB   13
#define ORDER_64MB   14
#define ORDER_128MB  15
#define ORDER_256MB  16
#define ORDER_512MB  17
#define ORDER_1GB    18

#define KATA_DEFAULT_PAGE_SIZE ( KB( 4 ) )

#define ORDER_TO_BYTES( order ) ( ( KATA_DEFAULT_PAGE_SIZE ) << order )

KataAllocator* kata_init();

void* kata_alloc( u64 order, boolean raw_mode );

void kata_free( u64 addr );
void kata_free_raw( u64 addr, u32 order );

void kata_populate_regions( BiosBootInfo* bios_boot_info );
void kata_add_region( u64 addr, u64 size );
void kata_add_block( u64 addr, u32 order );

#endif
