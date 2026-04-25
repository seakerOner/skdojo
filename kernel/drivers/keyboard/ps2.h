#ifndef PS2_KEYBOARD_H
#define PS2_KEYBOARD_H

#include "../../keyboard/keyboard_sensei.h"

#define PS2_SHIFT_L_PRESS 0x2A
#define PS2_SHIFT_R_PRESS 0x36
#define PS2_CTRL_PRESS    0x1D
#define PS2_ALT_PRESS     0x38

#define PS2_SHIFT_L_RELEASE 0xAA
#define PS2_SHIFT_R_RELEASE 0xB6
#define PS2_CTRL_RELEASE    0x9D
#define PS2_ALT_RELEASE     0xB8

#define PS2_EXT_SUPER_LEFT_PRESSED   0x5B
#define PS2_EXT_SUPER_RIGHT_PRESSED  0x5C
#define PS2_EXT_SUPER_LEFT_RELEASED  0xDB
#define PS2_EXT_SUPER_RIGHT_RELEASED 0xDC

KeyEvent ps2_keyboard_translate( unsigned char scancode );

#endif
