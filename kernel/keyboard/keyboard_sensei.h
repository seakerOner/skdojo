#ifndef KEYBOARD_SENSEI_H
#define KEYBOARD_SENSEI_H

#include "../inttype.h"
#define MAX_KEYBOARD_EVENTS 128

typedef struct {
    char key;       // raw
    char ascii;     // with modifiers

    u8 pressed;     // 1 = press, 0 = release

    u8 shift;
    u8 ctrl;
    u8 alt;
    u8 super;
    u8 fn;

    u8 __internal_extend;
} KeyEvent;

typedef struct {
    KeyEvent event_stack[MAX_KEYBOARD_EVENTS];
    u64 index;
    u64 stack_capacity;
} KeyboardSensei;

void keyboard_sensei_dispatch_event(KeyEvent ev);
i32  keyboard_has_events();
i32  keyboard_pop_event(KeyEvent* ev);

#endif
