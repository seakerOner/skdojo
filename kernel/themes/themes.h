#ifndef THEMES_H
#define THEMES_H

#include "../vga/vga_colors.h"

// Name of Dojo color Themes
typedef enum {
    THEME_UGLYDOJO,
    THEME_ZENMODE ,
} ThemesNames;

// structure of Dojo Theme for VGA
typedef struct {
    unsigned short main_colors;     // background, foreground and cursor colors
    unsigned short err_color;
    unsigned short success_color;
    unsigned short highlight_color;
    unsigned short comment_color;
} VgaThemeMetadata;

// different types of video output
typedef enum {
    VGA_MODE,
} VideoType;

// theme in use
typedef struct {
    VideoType video_type;
    ThemesNames name;
    char cursor;
    union {
        VgaThemeMetadata vga;
    } metadata;
} DojoTheme;

void      dojo_check_video_mode();
void      dojo_set_theme(ThemesNames theme);
void      dojo_clear_screen();
const DojoTheme* dojo_get_theme();

#endif
