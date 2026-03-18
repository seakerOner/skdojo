#include "interrupts/k_interrupts.h"
#include "vga/vga.h"

void kmain() {
    init_interrupts_x86();

    vga_clear_screen();

    vga_print("Welcome to the Dojo's Kernel \n");
    vga_print("---> Using VGA text mode \n");
}

