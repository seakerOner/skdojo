#include "vga_driver.h"
#include "vga.h"

void driver_vga_putc(void* fb, const char c, const StyleColor colors) {
    vga_videobuffer* vga = (vga_videobuffer *)fb;
    vga_putc(vga, c, colors);
}
void driver_vga_print(void* fb, const char* msg, const StyleColor colors) {
    vga_videobuffer* vga = (vga_videobuffer *)fb;
    vga_print(vga, msg, colors);
}
void driver_vga_clear(void* fb, const StyleColor colors) {
    vga_videobuffer* vga = (vga_videobuffer *)fb;
    vga_clear_screen(vga, colors);
}
void driver_vga_scroll(void* fb, const StyleColor colors) {
    vga_videobuffer* vga = (vga_videobuffer *)fb;
    vga_scroll(vga, colors);
}
void driver_vga_newline(void* fb, const StyleColor colors) {
    vga_videobuffer* vga = (vga_videobuffer *)fb;
    vga_newline(vga, colors);
}
void driver_vga_gotoline(void* fb, const u32 line, const StyleColor colors) {
    vga_videobuffer* vga = (vga_videobuffer *)fb;
    vga_gotoline(vga, line, colors);
}
void driver_vga_getcolumn(void* fb, u32* col) {
    *col = ((vga_videobuffer *)fb)->col;
}
u64  driver_vga_framebuffer_size() {
    return sizeof(vga_videobuffer);
};
void driver_vga_new_framebuffer(u32 start_row, u32 start_col, 
                            u32 max_collumns, u32 max_rows, 
                            void* out_fb) {
    vga_new_videobuffer(start_row, start_col, max_collumns, max_rows, (vga_videobuffer *)out_fb);
}
