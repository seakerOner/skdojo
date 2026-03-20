#ifndef THEMES_H
#define THEMES_H

#include "../inttype.h"
#include "../drivers/video/vga/vga_colors.h"

typedef struct {
    u32 value;
} Color;

typedef struct {
    Color fg;
    Color bg;
} StyleColor;

// Name of Dojo color Themes
typedef enum {
    THEME_UGLYDOJO,
    THEME_ZENMODE ,
    THEME_PURE ,
    THEME_DARKMODE,
} ThemesNames;

// structure of Dojo Theme for VGA
typedef struct {
    StyleColor main_colors;     // background, foreground and cursor colors
    StyleColor err_color;
    StyleColor success_color;
    StyleColor highlight_color;
    StyleColor comment_color;
} ThemePalette;

// theme in use
typedef struct {
    ThemesNames name;
    char cursor;
    ThemePalette palette;
} DojoTheme;

void             dojo_set_theme(ThemesNames theme);
void             dojo_clear_screen();
const DojoTheme* dojo_get_theme();

#endif
