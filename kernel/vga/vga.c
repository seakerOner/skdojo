#include "vga.h"

// its okay to have a global state because VGA is synchronous
vga_videobuffer vga =  {
    .index = 0,
    .row = 0,
    .vga_base = (volatile char*)0xB8000
};

void vga_clear_screen(const unsigned short colors) {
    volatile char* vga_ref = vga.vga_base;

    for (int x = 0; x < VGA_SIZE; x += 2) {
        *vga_ref++ = ' ';
        *vga_ref++ = colors;
    }

    vga.index = 0;
    vga.row = 0;
}

void vga_print(const char* msg, const unsigned short colors) {
    while (*msg) {
        if (*msg == '\n') {
            vga_newline(colors);
            msg++;
            continue;
        } 

        if (vga.row >= VGA_ROWS) 
            vga_scroll(colors);

        vga.vga_base[vga.index++] = *msg++;
        vga.vga_base[vga.index++] = colors;

        vga.row = vga.index / VGA_ROW_LEN;
    }
}

void vga_putc(const char character, const unsigned short colors) {
    if (character == '\n') {
        vga_newline(colors);
        return;
    } 

    if (vga.row >= VGA_ROWS) 
        vga_scroll(colors);

    vga.vga_base[vga.index++] = character;
    vga.vga_base[vga.index++] = colors;

    vga.row = vga.index / VGA_ROW_LEN;
}

void vga_gotoline(const int line, const unsigned short colors) {
    if (line >= VGA_ROWS) {
        vga_scroll(colors);
        return;
    }
    vga.row = line;
    vga.index = vga.row * VGA_ROW_LEN;
}

void vga_newline(const unsigned short colors) {
    vga_gotoline(vga.row + 1, colors);
}

void vga_scroll(const unsigned short colors) {
    volatile char* v = vga.vga_base;

    for (int x = 0; x < VGA_SIZE - VGA_ROW_LEN; x++) {
        v[x] = v[x + VGA_ROW_LEN];
    }

    for (int x = VGA_SIZE - VGA_ROW_LEN; x < VGA_SIZE; x += 2) {
        v[x] = ' ';
        v[x + 1] = colors;
    }

    vga.row = VGA_ROWS - 1;
    vga.index = vga.row * VGA_ROW_LEN;
}

