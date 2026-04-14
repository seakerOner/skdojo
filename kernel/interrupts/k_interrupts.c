#include "k_interrupts.h"

static inline unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

static volatile u64 ticks = 0;

volatile u64* get_time_ticker() {
    volatile u64* ptr = &ticks;
    return ptr;
}

// TIMER interrupt
void irq0_kernel_intrpt() {
    ticks++;
};

// PS/2 keyboard interrupt 
void irq1_kernel_intrpt() {
    KeyEvent event = ps2_keyboard_translate(inb(0x60));

    if (!event.__internal_extend) {
        keyboard_sensei_dispatch_event(event);
    }
}
