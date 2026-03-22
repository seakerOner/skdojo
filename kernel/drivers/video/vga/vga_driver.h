#ifndef VGA_DRIVER_H
#define VGA_DRIVER_H

#include "../../../themes/themes.h"
#include "vga.h"

void driver_vga_putc(void* fb, const char c, const StyleColor colors); 
void driver_vga_print(void* fb, const char* msg, const StyleColor colors); 
void driver_vga_clear(void* fb, const StyleColor colors); 
void driver_vga_scroll(void* fb, const StyleColor colors);
void driver_vga_newline(void* fb, const StyleColor colors); 
void driver_vga_gotoline(void* fb, const u32 line, const StyleColor colors); 
u64  driver_vga_framebuffer_size();
void driver_vga_new_framebuffer(u32 start_row, u32 start_col, 
                            u32 max_collumns, u32 max_rows, 
                            void* out_fb);

#endif
