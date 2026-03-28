#ifndef KEYBOARD_SENSEI_H
#define KEYBOARD_SENSEI_H

#include "../inttype.h"
#define MAX_KEYBOARD_EVENTS 128

typedef enum {
    KEY_NONE = 0,

    KEY_ESC,

    KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, 
    KEY_6, KEY_7, KEY_8, KEY_9, KEY_0,

    KEY_A, KEY_B, KEY_C, KEY_D, KEY_E,
    KEY_F, KEY_G, KEY_H, KEY_I, KEY_J,
    KEY_K, KEY_L, KEY_M, KEY_N, KEY_O,
    KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T,
    KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,

    KEY_ARROW_UP, KEY_ARROW_DOWN,
    KEY_ARROW_LEFT, KEY_ARROW_RIGHT,

    KEY_ENTER,
    KEY_BACKSPACE,
    KEY_TAB,
    KEY_SPACE,

    KEY_SHIFT,
    KEY_CTRL,
    KEY_ALT,
    KEY_SUPER,

    KEY_UNKNOWN
} KeyCode;

typedef struct {
    KeyCode key;       // raw
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

KeyboardSensei* get_keyboard_sensei();

void keyboard_sensei_dispatch_event(KeyEvent ev);

i32  keyboard_has_events();
i32  keyboard_pop_event(KeyEvent* ev);

#endif
