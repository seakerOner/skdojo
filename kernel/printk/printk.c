#include "../themes/themes.h"
#include "../video/video_sensei.h"
#include "../video/wmanager_sensei.h"
#include "./printk.h"

void printk(const char* msg) {
    const DojoTheme* theme = dojo_get_theme();
    VideoSensei* sensei = get_video_sensei();

    // sensei->driver.print(wmanager_get_focused()->framebuffer, msg, theme->palette.main_colors);
}

void printk_err(const char* msg) {
    const DojoTheme* theme = dojo_get_theme();
    VideoSensei* sensei = get_video_sensei();

    // sensei->driver.print(wmanager_get_focused()->framebuffer, msg, theme->palette.err_color);
}

void printk_warn(const char* msg) {
    const DojoTheme* theme = dojo_get_theme();
    VideoSensei* sensei = get_video_sensei();

    // sensei->driver.print(wmanager_get_focused()->framebuffer, msg, theme->palette.highlight_color);
}

void printk_succ(const char* msg) {
    const DojoTheme* theme = dojo_get_theme();
    VideoSensei* sensei = get_video_sensei();

    // sensei->driver.print(wmanager_get_focused()->framebuffer, msg, theme->palette.success_color);
}

void putck(const char character) {
    const DojoTheme* theme = dojo_get_theme();
    VideoSensei* sensei = get_video_sensei();

    // sensei->driver.putc(wmanager_get_focused()->framebuffer, character, theme->palette.main_colors);
}
