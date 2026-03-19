#ifndef VGA_H
#define VGA_H

void vga_clear_screen(); 

void vga_print(const char* msg);
void vga_putc(const char character);

void vga_newline();
void vga_gotoline(const int line);
void vga_scroll();

#define VGA_COLUMNS 80
#define VGA_ROWS 25
#define VGA_BYTES_PER_CHAR 2

#define VGA_SIZE (VGA_COLUMNS * VGA_ROWS * VGA_BYTES_PER_CHAR)   // 4000 bytes
#define VGA_ROW_LEN (VGA_COLUMNS * VGA_BYTES_PER_CHAR)           // 160 bytes

typedef struct {
    volatile char* vga_base;
    int index;
    int row;
} vga_videobuffer;

#endif
