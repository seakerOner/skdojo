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

DojoTerminal terminal_new(DojoWindow* window);

void terminal_print(DojoTerminal* terminal, char* string);
void terminal_putc(DojoTerminal* terminal, char c);

void terminal_newline(DojoTerminal* terminal);
void terminal_gotoline(DojoTerminal* terminal, const u32 line);
void terminal_scroll(DojoTerminal* terminal);



void terminal_poll_events(DojoTerminal* terminal);

#endif
