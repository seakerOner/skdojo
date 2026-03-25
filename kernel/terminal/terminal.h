#ifndef TERMINAL_H
#define TERMINAL_H

#include "../video/video_sensei.h"
#include "../video/wmanager_sensei.h"
#include "../keyboard/keyboard_sensei.h"

#define TERMINAL_MAX_HISTORY 500
#define TERMINAL_BUFFER_LEN  256

typedef struct {
    char lines[TERMINAL_MAX_HISTORY][TERMINAL_BUFFER_LEN];
    u32 capacity;
    u32 count;
    u32 line_len;
} TerminalHistory;

typedef struct {
    char data[TERMINAL_BUFFER_LEN];
    u32 len;
    u32 index;
    u32 cursor;

    u32 input_start_row;
    u32 input_start_col;
} TerminalInput;

typedef struct {
    DojoWindow*     window;
    // TODO: add terminal history when we have a memory model
    // TerminalHistory history;
    TerminalInput   input_buffer;

    u32 cursor_row;         // both for graphic display
    u32 cursor_col;         

    char cursor_char;      // from DojoTheme 
} DojoTerminal;

DojoTerminal terminal_new(DojoWindow* window);

void terminal_print(DojoTerminal* terminal, char* string);
void terminal_printDEC(DojoTerminal* terminal, u64 num);
void terminal_putc(DojoTerminal* terminal, char c);

void terminal_newline(DojoTerminal* terminal);
void terminal_gotoline(DojoTerminal* terminal, const u32 line);
void terminal_scroll(DojoTerminal* terminal);

void terminal_poll(DojoTerminal* terminal);

void terminal_render(DojoTerminal* terminal);

#endif
