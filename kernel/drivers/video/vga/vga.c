#include "vga.h"

void vga_new_videobuffer( u32 start_row, u32 start_col, 
                            u32 max_collumns, u32 max_rows, 
                            vga_videobuffer* out_fb ) {
    vga_videobuffer v_buffer;
    v_buffer.start_row = start_row;
    v_buffer.start_col = start_col;

    v_buffer.max_collumns = max_collumns;
    v_buffer.max_rows = max_rows;

    v_buffer.vga_base = ( volatile ascii* ) 0xB8000+HIGH_MEM_IDENTITY;

    *out_fb = v_buffer;
}


void vga_draw_cell( vga_videobuffer* vga, u32 row, u32 col, ascii c, StyleColor style ) {
    if (row > vga->max_rows || col > vga->max_collumns)
        return;

    u32 abs_row = vga->start_row + row;
    u32 abs_col = vga->start_col + col;

    u32 index = ( abs_row * VGA_COLUMNS + abs_col ) * VGA_BYTES_PER_CHAR;
    vga->vga_base[index]   = c;
    vga->vga_base[index+1] = (( style.bg.value & 0x0F ) << 4 ) | ( style.fg.value & 0x0F );
}

void vga_read_cell( vga_videobuffer* vga, u32 row, u32 col, ascii* out, StyleColor* style_out ) {
    if (row > vga->max_rows || col > vga->max_collumns)
        return;

    u32 abs_row = vga->start_row + row;
    u32 abs_col = vga->start_col + col;

    u32 index = ( abs_row * VGA_COLUMNS + abs_col ) * VGA_BYTES_PER_CHAR;
    *out = vga->vga_base[index];
    style_out->bg.value = vga->vga_base[index+1] >> 4 & 0x0F; 
    style_out->fg.value = vga->vga_base[index+1] & 0x0F; 

}

void vga_clear( vga_videobuffer* vga, const StyleColor colors ) {
    for ( u32 r = 0; r < vga->max_rows; r++ ) {
        for ( u32 x = 0; x < vga->max_collumns; x++ ) {
            vga_draw_cell( vga, r, x, ' ', colors );
        }
    }
}

