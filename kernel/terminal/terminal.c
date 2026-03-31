#include "terminal.h"

#define PROMP_OFFSET 1

void terminal_toggle_cursor(DojoTerminal* terminal) {
    StyleColor inverted_color;
    char c;

    comp_read_cell(
            terminal->frame,
            terminal->cursor_row,
            terminal->cursor_col,
            &c,
            &inverted_color);

    INVERT_COLOR(inverted_color)

    comp_draw_cell(
            terminal->frame,
            terminal->cursor_row,
            terminal->cursor_col,
            c,
            inverted_color
            );
}

static void terminal_history_add_c(TerminalHistory* h, char c) {
    u32 abs_idx = h->c_count++ % (h->line_len * h->line_capacity);
    h->data[abs_idx] = c;
}

int terminal_new(CompWinFrame* frame, DojoTerminal* t) {
    const DojoTheme* theme = dojo_get_theme();

    t->history.line_capacity       = TERMINAL_MAX_HISTORY;
    t->history.c_count          = 0;
    t->history.line_len       = TERMINAL_BUFFER_LEN;
    // TODO: use heap space not from the kernel heap itself
    t->history.data           = (char*)kheap_reserve(
                                CEIL_PAGES(t->history.line_capacity * t->history.line_len, 
                                            KB(4)));
    if (!t->history.data)
        return -1;
    t->frame                  = frame;
    t->frame->app             = t;
    t->frame->on_resize       = (void *)terminal_on_resize;
    t->cursor_char            = theme->cursor;
    t->cursor_row             = 0;
    t->cursor_col             = 0;
    t->input_buffer.cursor    = 0;
    t->input_buffer.index     = 0;
    t->input_buffer.len       = TERMINAL_BUFFER_LEN;
    t->input_buffer.input_start_row = 0;
    t->input_buffer.input_start_col = 0;

    for (u32 x = 0; x < t->input_buffer.len; x++) {
        t->input_buffer.data[x] = ' ';                // fill buffer with spaces
    }

    terminal_putc(t, '>');

    t->input_buffer.input_start_row = t->cursor_row;
    t->input_buffer.input_start_col = t->cursor_col;

    terminal_toggle_cursor(t);
    return 1;
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
         terminal_history_add_c(&terminal->history, c);
        return;
    } 

    // if reached end of collum
    if (terminal->cursor_col >= terminal->frame->width)
        terminal_newline(terminal);

    // if reached last row
    if (terminal->cursor_row >= terminal->frame->height) 
        terminal_scroll(terminal);

    const DojoTheme* theme = dojo_get_theme();

     terminal_history_add_c(&terminal->history, c);
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
                // TODO: interpret the input buffer
                u32 x= 0; 
                while (x < terminal->input_buffer.index) {
                    terminal_history_add_c(&terminal->history, terminal->input_buffer.data[x]);
                    x++;
                }
                terminal_putc(terminal, ev->ascii);

                terminal_print(terminal, "BUFFER OUTPUT: ");
                x = 0;
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

void terminal_on_resize(void* app, u32 w, u32 h) {
    DojoTerminal* t = app;
    UNUSED(w);
    UNUSED(h);
    // if (t->cursor_row >= h)
    //     t->cursor_row = h - 1;
    // if (t->cursor_col >= w)
    //     t->cursor_col = w - 1;
    
    terminal_render(t);
}

static inline u32 _terminal_get_history_start(DojoTerminal* t) {
    u32 max     = t->history.line_capacity * t->history.line_len;
    u32 count   = t->history.c_count;
    u32 row     = 0;
    u32 col     = 0;

    if (count == 0)
        return 0;

    u32 x = count;
    
    while (x > 0) {
        x--;

        char c = t->history.data[x % max];

        if (c == '\n') {
            row++;
            col = 0;
        } else {
            col++;
            if (col >= t->frame->width) {
                col = 0;
                row++;
            }
        }

        if (row >= t->frame->height)
            break;
    }

    return x;
}

void terminal_render(DojoTerminal* t) {
    comp_clear(t->frame, dojo_get_theme()->palette.main_colors);

    StyleColor colors = dojo_get_theme()->palette.main_colors;
    u32 row   = 0;
    u32 col   = 0;
    u32 max   = t->history.line_capacity * t->history.line_len;

    u32 start = _terminal_get_history_start(t);

    // draw history
    for (u32 i = start; i < t->history.c_count; i++) {
        u32 idx = i % max;
        char c  = t->history.data[idx];

        if (c == '\n') {
            row++;
            col = 0;
        } else {
            comp_draw_cell(t->frame, row, col, c, colors);
            col++;

            if (col >= t->frame->width) {
                col = 0;
                row++;
            }
        }

        if (row >= t->frame->height)
            break;
    }
    t->input_buffer.input_start_row = row;
    t->input_buffer.input_start_col = col;
    
    // draw input buffer
    terminal_redraw_buffer(t);

    terminal_toggle_cursor(t);
}
