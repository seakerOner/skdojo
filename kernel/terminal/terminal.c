#include "terminal.h"

DojoTerminal new_terminal(DojoWindow* window) {
    const DojoTheme* theme = dojo_get_theme();
    VideoSensei* sensei_v   = get_video_sensei();
    DojoTerminal term;

    term.window = window;
    term.cursor_char = theme->cursor;
    term.cursor_row  = 0;
    sensei_v->driver.getcolumn(window->framebuffer, &term.cursor_col);

    print_terminal(&term, ">");
    return term;
}

void print_terminal(DojoTerminal* terminal, char* string) {
    VideoSensei* sensei_v = get_video_sensei();
    const DojoTheme* theme = dojo_get_theme();

    sensei_v->driver.print(terminal->window->framebuffer, string, theme->palette.main_colors);
}

void putc_terminal(DojoTerminal* terminal, char c) {
    VideoSensei* sensei_v = get_video_sensei();
    const DojoTheme* theme = dojo_get_theme();

    sensei_v->driver.putc(terminal->window->framebuffer, c, theme->palette.main_colors);
}

void terminal_poll_events(DojoTerminal* terminal){
    while(keyboard_has_events()) {
        KeyEvent ev;
        if (!keyboard_pop_event(&ev)) {
            continue;
        }
        if (ev.pressed && ev.key == '1' && ev.shift && ev.super) {
            wmanager_focus((((char)ev.key) ^ '0') - 1);

            continue;
        }
        if (ev.pressed && ev.key == '2' && ev.shift && ev.super) {
            wmanager_focus((((char)ev.key) ^ '0') - 1);

            continue;
        }

        if (ev.pressed) {
            putc_terminal(terminal, ev.ascii);
            if (ev.ascii == '\n') {
                putc_terminal(terminal, '>');
            }
        }

    }
}
