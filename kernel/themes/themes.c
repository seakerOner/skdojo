#include "../video/video_sensei.h"
#include "../video/wmanager_sensei.h"

static DojoTheme dojo_theme;

static inline void set_vga_theme() {
    dojo_theme.cursor = '_';
    ThemePalette* _vga = &dojo_theme.palette;
    
    switch (dojo_theme.name) {
        case THEME_UGLYDOJO:
            _vga->main_colors.bg.value        = VGA_BLUE;
            _vga->main_colors.fg.value        = VGA_LIGHT_GRAY;
            _vga->success_color.bg.value      = VGA_BLUE;
            _vga->success_color.fg.value      = VGA_GREEN;
            _vga->err_color.bg.value          = VGA_BLUE;
            _vga->err_color.fg.value          = VGA_LIGHT_RED;
            _vga->highlight_color.bg.value    = VGA_BLUE;
            _vga->highlight_color.fg.value    = VGA_YELLOW;
            _vga->comment_color.bg.value      = VGA_BLUE;
            _vga->comment_color.fg.value      = VGA_BLACK;
            _vga->focus_color.bg.value        = VGA_BLUE;
            _vga->focus_color.fg.value        = VGA_DARK_GRAY;
            break;
        case THEME_ZENMODE:
            _vga->main_colors.bg.value        = VGA_LIGHT_GRAY;
            _vga->main_colors.fg.value        = VGA_BLACK;
            _vga->success_color.bg.value      = VGA_LIGHT_GRAY;
            _vga->success_color.fg.value      = VGA_GREEN;
            _vga->err_color.bg.value          = VGA_LIGHT_GRAY;
            _vga->err_color.fg.value          = VGA_RED;
            _vga->highlight_color.bg.value    = VGA_LIGHT_GRAY;
            _vga->highlight_color.fg.value    = VGA_DARK_GRAY;
            _vga->comment_color.bg.value      = VGA_LIGHT_GRAY;
            _vga->comment_color.fg.value      = VGA_BROWN;
            _vga->focus_color.bg.value        = VGA_LIGHT_GRAY;
            _vga->focus_color.fg.value        = VGA_WHITE;
            break;
        case THEME_PURE:
            _vga->main_colors.bg.value        = VGA_WHITE;
            _vga->main_colors.fg.value        = VGA_BLACK;
            _vga->success_color.bg.value      = VGA_WHITE;
            _vga->success_color.fg.value      = VGA_GREEN;
            _vga->err_color.bg.value          = VGA_WHITE;
            _vga->err_color.fg.value          = VGA_RED;
            _vga->highlight_color.bg.value    = VGA_WHITE;
            _vga->highlight_color.fg.value    = VGA_DARK_GRAY;
            _vga->comment_color.bg.value      = VGA_WHITE;
            _vga->comment_color.fg.value      = VGA_BROWN;
            _vga->focus_color.bg.value        = VGA_WHITE;
            _vga->focus_color.fg.value        = VGA_BLUE;
            break;
        case THEME_DARKMODE:
            _vga->main_colors.bg.value        = VGA_BLACK;
            _vga->main_colors.fg.value        = VGA_WHITE;
            _vga->success_color.bg.value      = VGA_BLACK;
            _vga->success_color.fg.value      = VGA_LIGHT_GREEN;
            _vga->err_color.bg.value          = VGA_BLACK;
            _vga->err_color.fg.value          = VGA_RED;
            _vga->highlight_color.bg.value    = VGA_BLACK;
            _vga->highlight_color.fg.value    = VGA_YELLOW;
            _vga->comment_color.bg.value      = VGA_BLACK;
            _vga->comment_color.fg.value      = VGA_BLUE;
            _vga->focus_color.bg.value        = VGA_BLACK;
            _vga->focus_color.fg.value        = VGA_LIGHT_CYAN;
            break;
        case THEME_MATRIX:
            _vga->main_colors.bg.value        = VGA_BLACK;
            _vga->main_colors.fg.value        = VGA_GREEN;
            _vga->success_color.bg.value      = VGA_BLACK;
            _vga->success_color.fg.value      = VGA_GREEN;
            _vga->err_color.bg.value          = VGA_BLACK;
            _vga->err_color.fg.value          = VGA_RED;
            _vga->highlight_color.bg.value    = VGA_BLACK;
            _vga->highlight_color.fg.value    = VGA_YELLOW;
            _vga->comment_color.bg.value      = VGA_BLACK;
            _vga->comment_color.fg.value      = VGA_LIGHT_BLUE;
            _vga->focus_color.bg.value        = VGA_BLACK;
            _vga->focus_color.fg.value        = VGA_LIGHT_GRAY;
            break;
        default:
            break;
    }

}

void dojo_set_theme(ThemesNames theme) {
    dojo_theme.name = theme;

    set_vga_theme();
}

const DojoTheme* dojo_get_theme() {
    return &dojo_theme;
}
