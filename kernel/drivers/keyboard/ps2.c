#include "./ps2.h"

// 
// PS/2 Scan Code Set 1 US layout
//

static unsigned char ps2_scanmap[128] = 
{
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t',
    'q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,
    'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,
    '\\',
    'z','x','c','v','b','n','m',',','.','/',
    0,
    '*',
    0,
    ' ',
};

static unsigned char ps2_scanmap_shift[128] = 
{
    0, 27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
    '\t',
    'Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
    0,
    'A','S','D','F','G','H','J','K','L',':','"','~',
    0,
    '|',
    'Z','X','C','V','B','N','M','<','>','?',
    0,
    '*',
    0,
    ' ',
};

static KeyCode ps2_keymap_table[128] = {
    [0x01] = KEY_ESC,

    [0x02] = KEY_1,
    [0x03] = KEY_2,
    [0x04] = KEY_3,
    [0x05] = KEY_4,
    [0x06] = KEY_5,
    [0x07] = KEY_6,
    [0x08] = KEY_7,
    [0x09] = KEY_8,
    [0x0A] = KEY_9,
    [0x0B] = KEY_0,

    [0x1E] = KEY_A,
    [0x30] = KEY_B,
    [0x2E] = KEY_C,
    [0x20] = KEY_D,
    [0x12] = KEY_E,
    [0x21] = KEY_F,
    [0x22] = KEY_G,
    [0x23] = KEY_H,
    [0x17] = KEY_I,
    [0x24] = KEY_J,
    [0x25] = KEY_K,
    [0x26] = KEY_L,
    [0x32] = KEY_M,
    [0x31] = KEY_N,
    [0x18] = KEY_O,
    [0x19] = KEY_P,
    [0x10] = KEY_Q,
    [0x13] = KEY_R,
    [0x1F] = KEY_S,
    [0x14] = KEY_T,
    [0x16] = KEY_U,
    [0x2F] = KEY_V,
    [0x11] = KEY_W,
    [0x2D] = KEY_X,
    [0x15] = KEY_Y,
    [0x2C] = KEY_Z,

    [0x48] = KEY_ARROW_UP,
    [0x50] = KEY_ARROW_DOWN,
    [0x4D] = KEY_ARROW_RIGHT,
    [0x4B] = KEY_ARROW_LEFT,

    [0x1C] = KEY_ENTER,
    [0x0E] = KEY_BACKSPACE,
    [0x0F] = KEY_TAB,
    [0x39] = KEY_SPACE
};

static int ps2_shift      = 0;
static int ps2_ctrl       = 0;
static int ps2_alt        = 0;
static int ps2_fn         = 0;
static int ps2_super      = 0;

// PS/2 keyboard interrupt 
KeyEvent ps2_keyboard_translate(unsigned char scancode) {
    KeyEvent event = {0};

    if (scancode == 0xE0) {
        event.__internal_extend = 1;
        return event;
    }

    if (scancode == PS2_SHIFT_L_PRESS || scancode == PS2_SHIFT_R_PRESS) {
        ps2_shift = 1;
        event.__internal_extend = 1;
        return event;
    }
    if (scancode == PS2_SHIFT_L_RELEASE || scancode == PS2_SHIFT_R_RELEASE) {
        ps2_shift = 0;
        event.__internal_extend = 1;
        return event;
    }

    if (scancode == PS2_CTRL_PRESS) {
        ps2_ctrl = 1;
        event.__internal_extend = 1;
        return event;
    }
    if (scancode == PS2_CTRL_RELEASE) {
        ps2_ctrl = 0;
        event.__internal_extend = 1;
        return event;
    }

    if (scancode == PS2_ALT_PRESS) {
        ps2_alt = 1;
        event.__internal_extend = 1;
        return event;
    }
    if (scancode == PS2_ALT_RELEASE) {
        ps2_alt = 0;
        event.__internal_extend = 1;
        return event;
    }
    u8 code = scancode & 0x7F;

    if (code > 128) {
        event.__internal_extend = 1;
        return event;
    }
    if (scancode == PS2_EXT_SUPER_LEFT_PRESSED) {
        ps2_super = 1;
        event.__internal_extend = 1;
        return event;
    }
    if (scancode == PS2_EXT_SUPER_LEFT_RELEASED) {
        ps2_super = 0;
        event.__internal_extend = 1;
        return event;
    }
    if (scancode == PS2_EXT_SUPER_RIGHT_PRESSED) {
        ps2_super = 1;
        event.__internal_extend = 1;
        return event;
    }
    if (scancode == PS2_EXT_SUPER_RIGHT_RELEASED) {
        ps2_super = 0;
        event.__internal_extend = 1;
        return event;
    }

    event.key = ps2_keymap_table[code];
    event.alt = ps2_alt;
    event.ctrl = ps2_ctrl;
    event.super = ps2_super;
    event.fn = ps2_fn;
    event.shift = ps2_shift;
    event.ascii = ps2_shift ? ps2_scanmap_shift[code] : ps2_scanmap[code];

    event.pressed = !(scancode & 0x80);

    return event;
}
