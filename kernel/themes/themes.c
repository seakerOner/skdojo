#include "../vga/vga.h"
#include "./themes.h"

DojoTheme dojo_theme;

void dojo_check_video_mode() {
    // TODO: Check VGA support via PCI
    // hardcoded to VGA for now
    dojo_theme.video_type = VGA_MODE;
    dojo_theme.cursor = '_';
}

void dojo_clear_screen() {
    switch (dojo_theme.video_type) {
        case VGA_MODE:
            vga_clear_screen(dojo_theme.metadata.vga.main_colors);
            break;
        default:
            break;
    }
}

static inline void set_vga_theme() {
    VgaThemeMetadata* _vga = &dojo_theme.metadata.vga;
    
    switch (dojo_theme.name) {
        case THEME_UGLYDOJO:
            _vga->main_colors     = VGA_BKG_BLUE | VGA_FGR_LIGHT_GRAY;
            _vga->success_color   = VGA_BKG_BLUE | VGA_FGR_GREEN;
            _vga->err_color       = VGA_BKG_BLUE | VGA_FGR_LIGHT_RED;
            _vga->highlight_color = VGA_BKG_BLUE | VGA_FGR_YELLOW;
            _vga->comment_color   = VGA_BKG_BLUE | VGA_FGR_BLACK;
            break;
        case THEME_ZENMODE:
            _vga->main_colors     = VGA_BKG_LIGHT_GRAY | VGA_FGR_BLACK;
            _vga->success_color   = VGA_BKG_LIGHT_GRAY | VGA_FGR_GREEN;
            _vga->err_color       = VGA_BKG_LIGHT_GRAY | VGA_FGR_RED;
            _vga->highlight_color = VGA_BKG_LIGHT_GRAY | VGA_FGR_DARK_GRAY;
            _vga->comment_color   = VGA_BKG_LIGHT_GRAY | VGA_FGR_BROWN;
            break;
        default:
            break;
    }

}

void dojo_set_theme(ThemesNames theme) {
    dojo_theme.name = theme;

    switch (dojo_theme.video_type) {
        case VGA_MODE:
            set_vga_theme();
            break;
        default:
            break;
    }
}

const DojoTheme* dojo_get_theme() {
    return &dojo_theme;
}
