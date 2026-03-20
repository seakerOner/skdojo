#include "vga.h"

static inline u32 vga_calc_index(vga_videobuffer* vga) {
    u32 abs_row = vga->start_row + vga->row;
    u32 abs_col = vga->start_col + vga->col;

    return (abs_row * VGA_COLUMNS + abs_col) * VGA_BYTES_PER_CHAR;
}

// TODO:
void vga_new_videobuffer(u32 start_row, u32 start_col, 
                            u32 max_collumns, u32 max_rows, 
                            vga_videobuffer* out_fb) {
    vga_videobuffer v_buffer;
    v_buffer.start_row = start_row;
    v_buffer.start_col = start_col;

    v_buffer.row = 0;
    v_buffer.col = 0;

    v_buffer.max_collumns = max_collumns;
    v_buffer.max_rows = max_rows;

    v_buffer.vga_base = (volatile char*)0xB8000;

    *out_fb = v_buffer;
}

void vga_clear_screen(vga_videobuffer* vga, const StyleColor colors) {
    volatile char* vga_ref = vga->vga_base;
    for (u32 r = 0; r < vga->max_rows; r++) {
        for (u32 x = 0; x < vga->max_collumns; x++) {
            u32 abs_row = vga->start_row + r;
            u32 abs_col = vga->start_col + x;

            u32 index = (abs_row * VGA_COLUMNS + abs_col) * VGA_BYTES_PER_CHAR;
            vga_ref[index] = ' ';
            vga_ref[index+1] = ((colors.bg.value & 0x0F) << 4) | (colors.fg.value & 0x0F);
        }
    }

    vga->row = 0;
    vga->col = 0;
}


void vga_putc(vga_videobuffer* vga, const char character, const StyleColor colors) {
    if (character == '\n') {
        vga_newline(vga, colors);
        return;
    } 

    // if reached end of collum
    if (vga->col >= vga->max_collumns)
        vga_newline(vga, colors);

    // if reached last row
    if (vga->row >= vga->max_rows) 
        vga_scroll(vga, colors);

    u32 index = vga_calc_index(vga);

    vga->vga_base[index] = character;
    vga->vga_base[index+1] = ((colors.bg.value & 0x0F) << 4) | (colors.fg.value & 0x0F); 

    vga->col++;
}

void vga_print(vga_videobuffer* vga, const char* msg, const StyleColor colors) {
    while (*msg) {
        vga_putc(vga, *msg++, colors);
    }
}
void vga_gotoline(vga_videobuffer* vga, const u32 line, const StyleColor colors) {
    if (line >= vga->max_rows) {
        vga_scroll(vga, colors);
        return;
    }
    vga->row = line;
    vga->col = 0;
}

void vga_newline(vga_videobuffer* vga, const StyleColor colors) {
    vga_gotoline(vga, vga->row + 1, colors);
}

void vga_scroll(vga_videobuffer* vga, const StyleColor colors) {
    volatile char* v = vga->vga_base;

    for (u32 r = 1; r < vga->max_rows; r++) {
        for (u32 x = 0; x < vga->max_collumns; x++) {
            u32 from_row = vga->start_row + r;
            u32 to_row = vga->start_row + (r - 1);

            u32 col = vga->start_col + x;

            u32 from_idx = (from_row * VGA_COLUMNS + col) * VGA_BYTES_PER_CHAR;
            u32 to_idx   = (to_row   * VGA_COLUMNS + col) * VGA_BYTES_PER_CHAR;

            vga->vga_base[to_idx]       = vga->vga_base[from_idx];
            vga->vga_base[to_idx + 1]   = vga->vga_base[from_idx + 1];
        }
    }

    u32 last_row = vga->start_row + vga->max_rows - 1;
    for (u32 c = 0; c < vga->max_collumns; c++) {
        u32 col = vga->start_col + c;
        u32 idx = (last_row * VGA_COLUMNS + col) * 2;

        vga->vga_base[idx] = ' ';
        vga->vga_base[idx + 1] =
            ((colors.bg.value & 0x0F) << 4) |
            (colors.fg.value & 0x0F);
    }
    vga->col = 0;
}

