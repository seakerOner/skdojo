#include "vga.h"

vga_videobuffer vga =  {
    .index = 0,
    .row = 0,
    .vga_base = (volatile char*)0xB8000
};

void vga_clear_screen() {
    volatile char* vga_ref = vga.vga_base;

    for (int x = 0; x < (VGA_CHARS / 2); x++) {
        *vga_ref++ = ' ';
        *vga_ref++ = 0x0F;
    }

    vga.index = 0;
    vga.row = 0;
}

void vga_print(const char* msg) {
    while (*msg) {
        *(vga.vga_base + vga.index++) = *msg++;
        *(vga.vga_base + vga.index++) = 0x0F;
    }
    vga.row = vga.index / VGA_ROW_LEN;
}

void vga_gotoline(const int line) {
    if (line > VGA_ROWS) {
        vga_print("[KERNEL ERROR] VGA row overflow");
    }
    vga.row = line;
    vga.index = vga.row * VGA_ROW_LEN;
}

void vga_newline() {
    vga_gotoline(vga.row + 1);
}


// #define VGA_CHARS 4000          // 2k chars but we counting the color char
// #define VGA_COLUMNS 8
// #define VGA_ROWS 25
// #define VGA_ROW_LEN VGA_MAX_CHARS / VGA_MAX_ROWS



