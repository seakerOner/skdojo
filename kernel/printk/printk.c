#include "../themes/themes.h"
#include "../video/video_sensei.h"
#include "../video/compositor_sensei.h"
#include "./printk.h"

void printk(const char* msg) {
    const DojoTheme* theme = dojo_get_theme();
    VideoSensei* sensei = get_video_sensei();

    sensei->driver.print(compositor_get_focused()->framebuffer, msg, theme->palette.main_colors);
}

void putck(const char character) {
    const DojoTheme* theme = dojo_get_theme();
    VideoSensei* sensei = get_video_sensei();

    sensei->driver.putc(compositor_get_focused()->framebuffer, character, theme->palette.main_colors);
}
