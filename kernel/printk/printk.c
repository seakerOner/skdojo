#include "./printk.h"

static ascii* printk_buffer = NULL;

void printk_init() {
}

void printk( const ascii* msg ) {
    UNUSED(msg);
    // const DojoTheme* theme = dojo_get_theme();
    // VideoSensei* sensei = get_video_sensei();

    // sensei->driver.print(wmanager_get_focused()->framebuffer, msg, theme->palette.main_colors);
}

void printk_err( const ascii* msg ) {
    UNUSED(msg);
    // const DojoTheme* theme = dojo_get_theme();
    // VideoSensei* sensei = get_video_sensei();

    // sensei->driver.print(wmanager_get_focused()->framebuffer, msg, theme->palette.err_color);
}

void printk_warn( const ascii* msg ) {
    UNUSED(msg);
    // const DojoTheme* theme = dojo_get_theme();
    // VideoSensei* sensei = get_video_sensei();

    // sensei->driver.print(wmanager_get_focused()->framebuffer, msg, theme->palette.highlight_color);
}

void printk_succ( const ascii* msg ) {
    UNUSED(msg);
    // const DojoTheme* theme = dojo_get_theme();
    // VideoSensei* sensei = get_video_sensei();

    // sensei->driver.print(wmanager_get_focused()->framebuffer, msg, theme->palette.success_color);
}

void putck( const ascii character ) {
    UNUSED(character);
    // const DojoTheme* theme = dojo_get_theme();
    // VideoSensei* sensei = get_video_sensei();

    // sensei->driver.putc(wmanager_get_focused()->framebuffer, character, theme->palette.main_colors);
}
