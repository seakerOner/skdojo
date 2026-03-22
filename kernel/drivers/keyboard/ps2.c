#include "./ps2.h"

unsigned char ps2_scanmap[128] = 
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

unsigned char ps2_scanmap_shift[128] = 
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

    event.key = ps2_scanmap[code];
    event.alt = ps2_alt;
    event.ctrl = ps2_ctrl;
    event.super = ps2_super;
    event.fn = ps2_fn;
    event.shift = ps2_shift;
    event.ascii = ps2_shift ? ps2_scanmap_shift[code] : ps2_scanmap[code];

    event.pressed = !(scancode & 0x80);



    return event;
}
