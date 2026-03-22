#ifndef K_INTERRUPTS
#define K_INTERRUPTS

#include "../drivers/keyboard/ps2.h"

extern void init_interrupts_x86();

// PS/2 keyboard interrupt 
void irq1_kernel_intrpt();

#endif
