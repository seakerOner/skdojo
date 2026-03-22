#ifndef TERMINAL_H
#define TERMINAL_H

#include "../video/video_sensei.h"
#include "../video/wmanager_sensei.h"
#include "../keyboard/keyboard_sensei.h"

typedef struct {
    DojoWindow* window;

    u32 cursor_row;
    u32 cursor_col;         

    char cursor_char;      // from DojoTheme 
} DojoTerminal;

DojoTerminal new_terminal(DojoWindow* window);

void print_terminal(DojoTerminal* terminal, char* string);
void putc_terminal(DojoTerminal* terminal, char c);

void terminal_poll_events(DojoTerminal* terminal);

#endif
