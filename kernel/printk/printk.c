#include "../themes/themes.h"
#include "../video/video_sensei.h"
#include "./printk.h"

void printk(const char* msg) {
    const DojoTheme* theme = dojo_get_theme();
    VideoSensei* sensei = get_video_sensei();

    sensei->driver.print(sensei->driver.get_framebuffer(), msg, theme->palette.main_colors);
}

void putck(const char character) {
    const DojoTheme* theme = dojo_get_theme();
    VideoSensei* sensei = get_video_sensei();

    sensei->driver.putc(sensei->driver.get_framebuffer(), character, theme->palette.main_colors);
}
