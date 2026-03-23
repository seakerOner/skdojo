#include "terminal.h"

DojoTerminal terminal_new(DojoWindow* window) {
    const DojoTheme* theme = dojo_get_theme();
    VideoSensei* sensei_v   = get_video_sensei();
    DojoTerminal term;

    term.window = window;
    term.cursor_char = theme->cursor;
    term.cursor_row  = 0;
    term.cursor_col  = 0;

    terminal_putc(&term, '>');
    return term;
}

void terminal_putc(DojoTerminal *terminal, char c) {
    if (c == '\n') {
        terminal_newline(terminal);
        return;
    } 

    // if reached end of collum
    if (terminal->cursor_col >= terminal->window->width)
        terminal_newline(terminal);

    // if reached last row
    if (terminal->cursor_row >= terminal->window->height) 
        terminal_scroll(terminal);

    VideoSensei* sensei_v   = get_video_sensei();
    const DojoTheme* theme = dojo_get_theme();

    sensei_v->driver.draw_cell(terminal->window->framebuffer, terminal->cursor_row, 
                                terminal->cursor_col, c, theme->palette.main_colors);

    terminal->cursor_col++;
}

void terminal_print(DojoTerminal* terminal, char* string) {
    VideoSensei* sensei_v = get_video_sensei();
    const DojoTheme* theme = dojo_get_theme();

    while (*string) {
        terminal_putc(terminal, *string++);
    }

}

void terminal_newline(DojoTerminal* terminal) {
    terminal_gotoline(terminal, terminal->cursor_row+ 1);
}

void terminal_gotoline(DojoTerminal* terminal, const u32 line) {
    if (line >= terminal->window->height) {
        terminal_scroll(terminal);
        return;
    }
    terminal->cursor_row = line;
    terminal->cursor_col = 0;
}

void terminal_scroll(DojoTerminal* terminal) {
    VideoSensei* sensei_v = get_video_sensei();
    const DojoTheme* theme = dojo_get_theme();

    for (u32 r = 1; r < terminal->window->height; r++) {
        for (u32 c = 0; c < terminal->window->width; c++) {
            char ch;
            StyleColor style;

            sensei_v->driver.read_cell(
                    terminal->window->framebuffer,
                    r,
                    c,
                    &ch,
                    &style
                    );

            sensei_v->driver.draw_cell(
                    terminal->window->framebuffer,
                    r - 1,
                    c,
                    ch,
                    style
                    );
        }
    }

    // clear last line
    for (u32 col = 0; col < terminal->window->width; col++) {
        sensei_v->driver.draw_cell(
                terminal->window->framebuffer,
                terminal->window->height - 1,
                col,
                ' ',
                theme->palette.main_colors
                );
    }

    terminal->cursor_row = terminal->window->height - 1;
    terminal->cursor_col = 0;
}

static inline void terminal_backspace(DojoTerminal* terminal) {
    VideoSensei* sensei_v = get_video_sensei();
    const DojoTheme* theme = dojo_get_theme();

    terminal->cursor_col--;
    terminal_putc(terminal, ' ');

    terminal->cursor_col--;
}

void terminal_poll_events(DojoTerminal* terminal){
    VideoSensei* sensei_v = get_video_sensei();
    const DojoTheme* theme = dojo_get_theme();

    while(keyboard_has_events()) {
        KeyEvent ev;
        if (!keyboard_pop_event(&ev)) {
            continue;
        }
        if (ev.pressed && ev.key == KEY_1 && ev.shift && ev.super) {
            wmanager_focus(0);
            continue;
        }
        if (ev.pressed && ev.key == KEY_2 && ev.shift && ev.super) {
            wmanager_focus(1);

            continue;
        }

        if (ev.pressed) {
            if (ev.key == KEY_BACKSPACE) {
                terminal_backspace(terminal);
                continue;
            }

            terminal_putc(terminal, ev.ascii);
            if (ev.key == KEY_ENTER) {
                terminal_putc(terminal, '>');
            }
        }

    }
}
