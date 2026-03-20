#include "../printk/printk.h"
#include "k_interrupts.h"

unsigned char irq1_scanmap[128] = 
{
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t',
    'q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,
    'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,
    '\\',
    'z','x','c','v','b','n','m',',','.','/',
    0,
    '*',
    0,
    ' ',
};

unsigned char irq1_scanmap_shift[128] = 
{
    0, 27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
    '\t',
    'Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
    0,
    'A','S','D','F','G','H','J','K','L',':','"','~',
    0,
    '|',
    'Z','X','C','V','B','N','M','<','>','?',
    0,
    '*',
    0,
    ' ',
};

static inline unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

static int irq1_shift = 0;
static int irq1_ctrl  = 0;

#define IRQ1_SHIFT_L_PRESS 0x2A
#define IRQ1_SHIFT_R_PRESS 0x36
#define IRQ1_CTRL_PRESS    0x1D

#define IRQ1_SHIFT_L_RELEASE 0xAA
#define IRQ1_SHIFT_R_RELEASE 0xB6
#define IRQ1_CTRL_RELEASE    0x9D

// PS/2 keyboard interrupt 
void irq1_kernel_intrpt() {
    unsigned char scancode;

    scancode = inb(0x60);

    if (scancode == IRQ1_SHIFT_L_PRESS || scancode == IRQ1_SHIFT_R_PRESS) 
        irq1_shift = 1;

    if (scancode == IRQ1_SHIFT_L_RELEASE || scancode == IRQ1_SHIFT_R_RELEASE) 
        irq1_shift = 0;

    if (scancode == IRQ1_CTRL_PRESS)
        irq1_ctrl = 1;

    if (scancode == IRQ1_CTRL_RELEASE)
        irq1_ctrl = 0;

    if (scancode == 0xE0) {
        // TODO: special key with prefix
        return;
    }

    if (scancode & 0x80) {
        // TODO: key release
        return;
    }

    // key press 
    unsigned char key;

    if (irq1_shift)
        key = irq1_scanmap_shift[scancode];
    else 
        key = irq1_scanmap[scancode];

    if (irq1_ctrl && key >= 'a' && key <= 'z')
        key = key - 'a' + 1;

    if (key)
        putck(key);

}
