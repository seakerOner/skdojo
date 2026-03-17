#ifndef VGA_H
#define VGA_H

void vga_clear_screen(); 
void vga_print(const char* msg);
void vga_newline();
void vga_gotoline(const int line);

#define VGA_CHARS 4000          // 2k chars but we counting the color char
#define VGA_COLUMNS 8
#define VGA_ROWS 25

#define VGA_ROW_LEN (VGA_CHARS / VGA_ROWS)

typedef struct {
    volatile char* vga_base;
    int index;
    int row;
} vga_videobuffer;

#endif
