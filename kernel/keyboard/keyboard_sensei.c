#include "./keyboard_sensei.h"

static KeyboardSensei keyboard_sensei = {
    .stack_capacity = MAX_KEYBOARD_EVENTS,
    .index = 0,
};

KeyboardSensei* get_keyboard_sensei() {
    return &keyboard_sensei;
}

void keyboard_sensei_dispatch_event(KeyEvent ev) {
    u64 abs_index = keyboard_sensei.index % keyboard_sensei.stack_capacity;

    keyboard_sensei.event_stack[abs_index] = ev;
    keyboard_sensei.index++;
}

i32 keyboard_has_events() {
    return keyboard_sensei.index % keyboard_sensei.stack_capacity;
};

i32 keyboard_pop_event(KeyEvent* ev) {
    u64 abs_index = keyboard_sensei.index % keyboard_sensei.stack_capacity;
    if (abs_index == 0) {
        ev = NULL;
        return 0;
    }
    
    abs_index--;
    keyboard_sensei.index--;
    *ev = keyboard_sensei.event_stack[abs_index];
    return 1;
}

