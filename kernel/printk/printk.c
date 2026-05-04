#include "./printk.h"

static PrintkBuffer printk_buffer = {0};

void printk_init() {
    printk_buffer.data     = ( ascii* )kata_alloc(PRINTK_BUF_DYN_ORDER, TRUE);
    printk_buffer.capacity = KB(4) << PRINTK_BUF_DYN_ORDER;
    printk_buffer.idx      = 0;
}

static inline void _insert_kprint_buffer( const ascii character) {
    u64 abs_idx = printk_buffer.idx++ % printk_buffer.capacity;
    printk_buffer.data[abs_idx] = character;
}

void printk( const ascii* msg ) {
    ascii* info = KSTR( "[INFO]" );
    while ( *info ) 
        _insert_kprint_buffer( *info++ );

    while ( *msg )
        _insert_kprint_buffer( *msg++ );
}

void printk_err( const ascii* msg ) {
    ascii* err = KSTR( "[ERROR]" );
    while ( *err ) 
        _insert_kprint_buffer( *err++ );

    while ( *msg )
        _insert_kprint_buffer( *msg++ );
}

void printk_warn( const ascii* msg ) {
    ascii* warn = KSTR( "[WARNING]" );
    while ( *warn ) 
        _insert_kprint_buffer( *warn++ );

    while ( *msg )
        _insert_kprint_buffer( *msg++ );
}

void printk_succ( const ascii* msg ) {
    ascii* succ = KSTR( "[SUCCESS]" );
    while ( *succ ) 
        _insert_kprint_buffer( *succ++ );

    while ( *msg )
        _insert_kprint_buffer( *msg++ );
}
