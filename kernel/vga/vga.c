#include "vga.h"

vga_videobuffer vga =  {
    .index = 0,
    .row = 0,
    .vga_base = (volatile char*)0xB8000
};

void vga_clear_screen() {
    volatile char* vga_ref = vga.vga_base;

    for (int x = 0; x < VGA_SIZE; x += 2) {
        *vga_ref++ = ' ';
        *vga_ref++ = 0x0F;
    }

    vga.index = 0;
    vga.row = 0;
}

void vga_print(const char* msg) {
    while (*msg) {
        if (*msg == '\n') {
            vga_newline();
            msg++;
            continue;
        } 

        if (vga.row >= VGA_ROWS) 
            vga_scroll();

        vga.vga_base[vga.index++] = *msg++;
        vga.vga_base[vga.index++] = 0x0F;

        vga.row = vga.index / VGA_ROW_LEN;
    }
}

void vga_putc(const char character) {
    if (character == '\n') {
        vga_newline();
        return;
    } 

    if (vga.row >= VGA_ROWS) 
        vga_scroll();

    vga.vga_base[vga.index++] = character;
    vga.vga_base[vga.index++] = 0x0F;

    vga.row = vga.index / VGA_ROW_LEN;
}

void vga_gotoline(const int line) {
    if (line >= VGA_ROWS) {
        vga_scroll();
        return;
    }
    vga.row = line;
    vga.index = vga.row * VGA_ROW_LEN;
}

void vga_newline() {
    vga_gotoline(vga.row + 1);
}

void vga_scroll() {
    volatile char* v = vga.vga_base;

    for (int x = 0; x < VGA_SIZE - VGA_ROW_LEN; x++) {
        v[x] = v[x + VGA_ROW_LEN];
    }

    for (int x = VGA_SIZE - VGA_ROW_LEN; x < VGA_SIZE; x += 2) {
        v[x] = ' ';
        v[x + 1] = 0x0F;
    }

    vga.row = VGA_ROWS - 1;
    vga.index = vga.row * VGA_ROW_LEN;
}

