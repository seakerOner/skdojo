#include "../themes/themes.h"
#include "../vga/vga.h"
#include "./printk.h"

void printk(const char* msg) {
    const DojoTheme* theme = dojo_get_theme();

    switch (theme->video_type) {
        case VGA_MODE:
            vga_print(msg, theme->metadata.vga.main_colors);
            break;
        default:
            break;
    }
}

void putck(const char character) {
    const DojoTheme* theme = dojo_get_theme();

    switch (theme->video_type) {
        case VGA_MODE:
            vga_putc(character, theme->metadata.vga.main_colors);
            break;
        default:
            break;
    }
}
