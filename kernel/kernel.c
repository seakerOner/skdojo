#include "vga.h"

void kmain() {
    vga_clear_screen();
    vga_print("Welcome to the Dojo's Kernel");
    vga_newline();
    vga_print("---> Using VGA text mode");
    vga_newline();
    
    while (1);
}
