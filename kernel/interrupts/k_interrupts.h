#ifndef K_INTERRUPTS
#define K_INTERRUPTS

#include "../drivers/keyboard/ps2.h"

#define TICKS_PER_SECOND 800
#define MS_TO_TICKS(ms) ((ms * TICKS_PER_SECOND) / 1000)
volatile u64* get_time_ticker();

extern void init_interrupts_x86();

// TIMER interrupt
void irq0_kernel_intrpt();
// PS/2 keyboard interrupt 
void irq1_kernel_intrpt();

#endif
