#ifndef VGA_H
#define VGA_H

//
// vga colors
//
#include "./vga_colors.h"

void vga_clear_screen(const unsigned short colors); 

void vga_print(const char* msg, const unsigned short colors);
void vga_putc(const char character, const unsigned short colors);

void vga_newline(const unsigned short colors);
void vga_gotoline(const int line, const unsigned short colors);
void vga_scroll(const unsigned short colors);

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
