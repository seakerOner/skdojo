#ifndef TERMINAL_H
#define TERMINAL_H

#include "../video/video_sensei.h"
#include "../video/wmanager_sensei.h"
#include "../keyboard/keyboard_sensei.h"
#include "../video/compositor_sensei.h"
#include "../process/processes_sensei.h"
#include "../memory/kheap.h"

#define TERMINAL_MAX_HISTORY 512
#define TERMINAL_BUFFER_LEN  256

typedef struct {
    char* data;
    u32 c_count;
    u32 line_capacity;
    u32 line_len;
    u32 scroll_offset;
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
    CompWinFrame*   frame;
    TerminalHistory history;
    TerminalInput   input_buffer;

    u32 cursor_row;         // both for graphic display
    u32 cursor_col;         

    char cursor_char;      // from DojoTheme 
} DojoTerminal;

typedef struct {
    DojoProcess* base;
    DojoTerminal terminal;
} TerminalProcess;

DojoTerminal* terminal_new(CompWinFrame* frame, DojoTerminal* t);
//DojoTerminal* terminal_new(CompWinFrame* frame);

void terminal_print(DojoTerminal* terminal, char* string);
void terminal_printDEC(DojoTerminal* terminal, u64 num);
void terminal_putc(DojoTerminal* terminal, char c);

void terminal_newline(DojoTerminal* terminal);
void terminal_gotoline(DojoTerminal* terminal, const u32 line);
void terminal_scroll(DojoTerminal* terminal);

void terminal_event(void* terminal, KeyEvent* ev);
void terminal_on_resize(void* app, u32 w, u32 h);

void terminal_destroy(void* terminal);
void terminal_render(void* terminal);

#endif
