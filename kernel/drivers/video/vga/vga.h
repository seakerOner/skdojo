#ifndef VGA_H
#define VGA_H

//
// vga colors
//
#include "./vga_colors.h"
#include "../../../themes/themes.h"
#include "../../../inttype.h"
#include "../../../memory/memory_sensei.h"

typedef struct {
    volatile ascii* vga_base;
    u32 start_row;
    u32 start_col;

    u32 max_collumns;
    u32 max_rows;
} vga_videobuffer;

void vga_new_videobuffer( u32 start_row, u32 start_col, 
                            u32 max_collumns, u32 max_rows, 
                            vga_videobuffer* out );

void vga_clear( vga_videobuffer* vga, const StyleColor colors ); 

void vga_draw_cell( vga_videobuffer* vga, u32 row, u32 col, ascii c, StyleColor style );
void vga_read_cell( vga_videobuffer* vga, u32 row, u32 col, ascii* out, StyleColor* style_out );

#define VGA_COLUMNS 80
#define VGA_ROWS 25
#define VGA_BYTES_PER_CHAR 2

#define VGA_SIZE (VGA_COLUMNS * VGA_ROWS * VGA_BYTES_PER_CHAR)   // 4000 bytes
#define VGA_ROW_LEN (VGA_COLUMNS * VGA_BYTES_PER_CHAR)           // 160 bytes


#endif
