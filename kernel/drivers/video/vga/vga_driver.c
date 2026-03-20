#include "vga_driver.h"

vga_videobuffer videobuffer = {
    .row = 0,
    .max_collumns = VGA_COLUMNS,
    .max_rows = VGA_ROWS,
    .index = 0,
    .vga_base = (volatile char*)0xB8000,
};

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
void* driver_vga_get_framebuffer() {
    return &videobuffer;
}
