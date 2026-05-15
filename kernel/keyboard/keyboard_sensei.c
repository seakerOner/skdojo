#include "./keyboard_sensei.h"
#include "../printk/printk.h"

static KeyboardSensei keyboard_sensei = {0};

void keyboard_sensei_init() {
    keyboard_sensei.count     = 0;
    keyboard_sensei.head      = 0;
    keyboard_sensei.tail      = 0;
    keyboard_sensei.tail      = 0;
    keyboard_sensei.capacity  = ORDER_TO_BYTES( MAX_KEYBOARD_EVENTS_ORDER ) / sizeof( KeyEvent );
    void* ptr = kata_alloc( MAX_KEYBOARD_EVENTS_ORDER, TRUE );

    if ( ptr == NULL ) {
        printk_err( KSTR( "[KEYBOARD][SENSEI] Failed to Kata alloc event stack\n" ) );
        // TODO: 
        // Create a fallback 
        return;
    }

    keyboard_sensei.events     = ( KeyEvent* )ptr;
}

KeyboardSensei* get_keyboard_sensei() {
    return &keyboard_sensei;
}

void keyboard_sensei_dispatch_event( KeyEvent ev ) {
    u64* head = &keyboard_sensei.head;

    keyboard_sensei.events[*head] = ev;
    *head = ( *head + 1 ) % keyboard_sensei.capacity;

    keyboard_sensei.count++;
}

i32 keyboard_has_events() {
    return keyboard_sensei.count > 0;
};

i32 keyboard_pop_event(KeyEvent* ev) {
    if ( keyboard_sensei.count == 0 ) 
        return 0;

    u64* tail = &keyboard_sensei.tail;

    *ev = keyboard_sensei.events[*tail];
    *tail = ( *tail + 1 ) % keyboard_sensei.capacity;

    keyboard_sensei.count--;

    return 1;
}

