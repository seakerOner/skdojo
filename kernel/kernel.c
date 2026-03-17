#include "vga.h"

void kmain() {
    vga_clear_screen();

    vga_print("Welcome to the Dojo's Kernel \n");
    vga_print("---> Using VGA text mode \n");
    
}
