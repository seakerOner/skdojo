#include "vga.h"

// TODO:
vga_videobuffer vga_new_videobuffer(u32 start_row, u32 max_collumns, u32 max_rows) {
    vga_videobuffer v_buffer;
    v_buffer.row = start_row;
    v_buffer.max_collumns = max_collumns;
    v_buffer.max_rows = max_rows;
    v_buffer.index = 0;
    v_buffer.vga_base = (volatile char*)0xB8000;

    return v_buffer;
}

void vga_clear_screen(vga_videobuffer* vga, const StyleColor colors) {
    volatile char* vga_ref = vga->vga_base;

    for (int x = 0; x < VGA_SIZE; x += 2) {
        *vga_ref++ = ' ';
        *vga_ref++ = ((colors.bg.value & 0x0F) << 4) | (colors.fg.value & 0x0F);
    }

    vga->index = 0;
    vga->row = 0;
}

void vga_print(vga_videobuffer* vga, const char* msg, const StyleColor colors) {
    while (*msg) {
        if (*msg == '\n') {
            vga_newline(vga, colors);
            msg++;
            continue;
        } 

        if (vga->row >= VGA_ROWS) 
            vga_scroll(vga, colors);

        vga->vga_base[vga->index++] = *msg++;
        vga->vga_base[vga->index++] = ((colors.bg.value & 0x0F) << 4) | (colors.fg.value & 0x0F);

        vga->row = vga->index / VGA_ROW_LEN;
    }
}

void vga_putc(vga_videobuffer* vga, const char character, const StyleColor colors) {
    if (character == '\n') {
        vga_newline(vga, colors);
        return;
    } 

    if (vga->row >= VGA_ROWS) 
        vga_scroll(vga, colors);

    vga->vga_base[vga->index++] = character;
    vga->vga_base[vga->index++] = ((colors.bg.value & 0x0F) << 4) | (colors.fg.value & 0x0F); 

    vga->row = vga->index / VGA_ROW_LEN;
}

void vga_gotoline(vga_videobuffer* vga, const u32 line, const StyleColor colors) {
    if (line >= VGA_ROWS) {
        vga_scroll(vga, colors);
        return;
    }
    vga->row = line;
    vga->index = vga->row * VGA_ROW_LEN;
}

void vga_newline(vga_videobuffer* vga, const StyleColor colors) {
    vga_gotoline(vga, vga->row + 1, colors);
}

void vga_scroll(vga_videobuffer* vga, const StyleColor colors) {
    volatile char* v = vga->vga_base;

    for (int x = 0; x < VGA_SIZE - VGA_ROW_LEN; x++) {
        v[x] = v[x + VGA_ROW_LEN];
    }

    for (int x = VGA_SIZE - VGA_ROW_LEN; x < VGA_SIZE; x += 2) {
        v[x] = ' ';
        v[x + 1] = ((colors.bg.value & 0x0F) << 4) | (colors.fg.value & 0x0F); 
    }

    vga->row = VGA_ROWS - 1;
    vga->index = vga->row * VGA_ROW_LEN;
}

