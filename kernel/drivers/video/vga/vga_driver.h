#ifndef VGA_DRIVER_H
#define VGA_DRIVER_H

#include "../../../themes/themes.h"
#include "vga.h"

void driver_vga_draw_cell( void* fb, u32 row, u32 col, char c, StyleColor style );
void driver_vga_read_cell( void* fb, u32 row, u32 col, char* out, StyleColor* style_out );
void driver_vga_clear( void* fb, const StyleColor colors ); 

u64  driver_vga_framebuffer_size();
void driver_vga_new_framebuffer( u32 start_row, u32 start_col, 
        u32 max_collumns, u32 max_rows, 
        void* out_fb );

#endif
