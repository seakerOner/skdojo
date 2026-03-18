#include "../vga/vga.h"
#include "k_interrupts.h"

void irq1_kernel_intrpt() {
    vga_print("OLAAAAAAA PAREI A CPU PARA ISTO! \n");
}
