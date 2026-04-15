#include "vga_driver.h"
#include "vga.h"

u64 driver_vga_framebuffer_size() {
    return sizeof(vga_videobuffer);
};

void driver_vga_new_framebuffer(u32 start_row, u32 start_col, 
        u32 max_collumns, u32 max_rows, 
        void* out_fb) {
    vga_new_videobuffer(start_row, start_col, max_collumns, max_rows, (vga_videobuffer *)out_fb);
}

void driver_vga_draw_cell(void* fb, u32 row, u32 col, char c, StyleColor style) {
    vga_draw_cell((vga_videobuffer *)fb, row, col, c, style);
}
void driver_vga_read_cell(void* fb, u32 row, u32 col, char* out, StyleColor* style_out) {
    vga_read_cell((vga_videobuffer *)fb, row, col, out, style_out);
}
void driver_vga_clear(void* fb, const StyleColor colors) {
    vga_clear((vga_videobuffer *)fb, colors);
}
