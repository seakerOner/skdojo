#include "terminal.h"

#define PROMP_OFFSET 1

void terminal_toggle_cursor(DojoTerminal* terminal) {
    VideoSensei* sensei_v = get_video_sensei();

    StyleColor inverted_color;
    char c;

    comp_read_cell(
            terminal->frame,
            terminal->cursor_row,
            terminal->cursor_col,
            &c,
            &inverted_color);

    inverted_color.bg.value ^= inverted_color.fg.value;
    inverted_color.fg.value = inverted_color.bg.value ^ inverted_color.fg.value;
    inverted_color.bg.value ^= inverted_color.fg.value;

    comp_draw_cell(
            terminal->frame,
            terminal->cursor_row,
            terminal->cursor_col,
            c,
            inverted_color
            );
}

DojoTerminal terminal_new(CompWinFrame* frame) {
    const DojoTheme* theme = dojo_get_theme();
    DojoTerminal term;

    term.frame                  = frame;
    term.cursor_char            = theme->cursor;
    term.cursor_row             = 0;
    term.cursor_col             = 0;
    // term.history.capacity       = TERMINAL_MAX_HISTORY;
    // term.history.count          = 0;
    // term.history.line_len       = TERMINAL_BUFFER_LEN;
    term.input_buffer.cursor    = 0;
    term.input_buffer.index     = 0;
    term.input_buffer.len       = TERMINAL_BUFFER_LEN;
    term.input_buffer.input_start_row = frame->start_height;
    term.input_buffer.input_start_col = frame->start_width;

    for (u32 x = 0; x < term.input_buffer.len; x++) {
        term.input_buffer.data[x] = ' ';                // fill buffer with spaces
    }

    terminal_putc(&term, '>');

    term.input_buffer.input_start_row = term.cursor_row;
    term.input_buffer.input_start_col = term.cursor_col;

    terminal_toggle_cursor(&term);
    return term;
}


void terminal_printDEC(DojoTerminal* terminal, u64 num) {
    char buffer[32] = {0};
    int x = 0;

    if (num == 0) {
        terminal_putc(terminal, '0');
        return;
    }

    while (num > 0) {
        buffer[x++] = (num%10) | '0';
        num /= 10;
    }

    while (x--) 
        terminal_putc(terminal, buffer[x]);

    terminal->input_buffer.input_start_row = terminal->cursor_row;
    terminal->input_buffer.input_start_col = terminal->cursor_col;
}


void terminal_putc(DojoTerminal *terminal, char c) {
    if (c == '\n') {
        terminal_newline(terminal);
        return;
    } 

    // if reached end of collum
    if (terminal->cursor_col >= terminal->frame->width)
        terminal_newline(terminal);

    // if reached last row
    if (terminal->cursor_row >= terminal->frame->height) 
        terminal_scroll(terminal);

    const DojoTheme* theme = dojo_get_theme();

    comp_draw_cell(terminal->frame, terminal->cursor_row, 
                                terminal->cursor_col, c, theme->palette.main_colors);

    terminal->cursor_col++;

    terminal->input_buffer.input_start_row = terminal->cursor_row;
    terminal->input_buffer.input_start_col = terminal->cursor_col;
}

void terminal_print(DojoTerminal* terminal, char* string) {
    while (*string) {
        terminal_putc(terminal, *string++);
    }
}

void terminal_newline(DojoTerminal* terminal) {
    terminal_gotoline(terminal, terminal->cursor_row+ 1);
}

void terminal_gotoline(DojoTerminal* terminal, const u32 line) {
    if (line >= terminal->frame->height) {
        terminal_scroll(terminal);
        return;
    }
    terminal->cursor_row = line;
    terminal->cursor_col = 0;
}

void terminal_scroll(DojoTerminal* terminal) {
    const DojoTheme* theme = dojo_get_theme();

    for (u32 r = 1; r < terminal->frame->height; r++) {
        for (u32 c = 0; c < terminal->frame->width; c++) {
            char ch;
            StyleColor style;

            comp_read_cell(
                    terminal->frame,
                    r,
                    c,
                    &ch,
                    &style
                    );

            comp_draw_cell(
                    terminal->frame,
                    r - 1,
                    c,
                    ch,
                    style
                    );
        }
    }

    // clear last line
    for (u32 col = 0; col < terminal->frame->width; col++) {
        comp_draw_cell(
                terminal->frame,
                terminal->frame->height - 1,
                col,
                ' ',
                theme->palette.main_colors
                );
    }

    terminal->cursor_row = terminal->frame->height - 1;
    terminal->cursor_col = 0;
}

static void terminal_redraw_buffer(DojoTerminal* t) {
    const DojoTheme* theme = dojo_get_theme();

    u32 width = t->frame->width;

    for (u32 x = 0; x <= t->input_buffer.index; x++) {

        u32 abs = t->input_buffer.input_start_col + x;

        u32 row = t->input_buffer.input_start_row + (abs/width);
        u32 col = abs % width;

        while (row >= t->frame->height) {
            terminal_scroll(t);
            t->input_buffer.input_start_row--;
            row--;
        }

        char c = (x < t->input_buffer.index)
            ? t->input_buffer.data[x]
            : ' ';  // clean last char
        
        comp_draw_cell(
                t->frame,
                row,
                col,
                c,
                theme->palette.main_colors
        );
    }
    u32 abs = t->input_buffer.input_start_col + t->input_buffer.cursor;

    t->cursor_row = t->input_buffer.input_start_row + (abs / width);
    t->cursor_col = abs % width;
}

static inline void terminal_addto_buffer(DojoTerminal* terminal, char c) {
    if (terminal->input_buffer.index >= terminal->input_buffer.len) {
        return;
    }

    // adding inside the buffer, we must offset the content of the buffer after the cursor
    if (terminal->input_buffer.cursor < terminal->input_buffer.index) {
        u32 to_shift = (terminal->input_buffer.index - terminal->input_buffer.cursor);
        char* end    = &terminal->input_buffer.data[terminal->input_buffer.index - 1];

        for (u32 x = 0; x < to_shift; x++) {
            *(end+1) = *end;
            end--;
        }
    } 
    terminal->input_buffer.data[terminal->input_buffer.cursor++] = c;
    terminal->input_buffer.index++;

    terminal_redraw_buffer(terminal);
}

static inline void terminal_backspace(DojoTerminal* terminal) {
    if (terminal->input_buffer.cursor == 0 || terminal->input_buffer.index == 0)
        return;

    terminal->input_buffer.cursor--;

    for (u32 i = terminal->input_buffer.cursor; i < terminal->input_buffer.index - 1; i++) 
        terminal->input_buffer.data[i] = terminal->input_buffer.data[i+1];

    terminal->input_buffer.index--;

    terminal_redraw_buffer(terminal);
}

static void terminal_cursor_move_back(DojoTerminal* terminal) {
    if (terminal->input_buffer.cursor == 0)
        return;

    terminal->input_buffer.cursor--;

    if (terminal->cursor_col == 0) {
        terminal->cursor_row--;
        terminal->cursor_col = terminal->frame->width - 1;
        return;
    }

    terminal->cursor_col--;
}

static inline void terminal_cursor_move_front(DojoTerminal* terminal) {
    if (terminal->input_buffer.cursor == terminal->input_buffer.index)
        return;

    terminal->input_buffer.cursor++;

    if (terminal->cursor_col == terminal->frame->width) {
        terminal->cursor_row++;
        terminal->cursor_col = 0;
        return;
    }

    terminal->cursor_col++;

}

void terminal_poll(DojoTerminal* terminal, KeyEvent* ev){
        if (ev->pressed && ev->key == KEY_ARROW_LEFT) {
            terminal_toggle_cursor(terminal);
            terminal_cursor_move_back(terminal);
            terminal_toggle_cursor(terminal);
            return;
        }
        if (ev->pressed && ev->key == KEY_ARROW_RIGHT) {
            terminal_toggle_cursor(terminal);
            terminal_cursor_move_front(terminal);
            terminal_toggle_cursor(terminal);
            return;
        }

        if (ev->pressed) {
            terminal_toggle_cursor(terminal);

            if (ev->key == KEY_ENTER) {
                terminal_putc(terminal, ev->ascii);
                // TODO: interpret the input buffer
                u32 x= 0;
                terminal_print(terminal, "BUFFER OUTPUT: ");
                while (x < terminal->input_buffer.index) {
                    terminal_putc(terminal, terminal->input_buffer.data[x]);
                    x++;
                }
                terminal_putc(terminal, '\n');

                terminal->input_buffer.cursor = 0;
                terminal->input_buffer.index  = 0;
                terminal->input_buffer.input_start_row = terminal->cursor_row;
                terminal->input_buffer.input_start_col = terminal->cursor_col;

                terminal_putc(terminal, '>');
                terminal_toggle_cursor(terminal);
                return;
            }

            if (ev->key == KEY_BACKSPACE) {
                terminal_backspace(terminal);
                terminal_toggle_cursor(terminal);
                return;
            }

            terminal_addto_buffer(terminal, ev->ascii);
            terminal_toggle_cursor(terminal);
        }
}
