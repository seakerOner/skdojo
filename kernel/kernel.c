#include "interrupts/k_interrupts.h"
#include "themes/themes.h"
#include "printk/printk.h"

// TODO: (Memory Managment)
//   Use the e820 RAM information from bootloader to know how many usable memory to page

void kmain() {
    init_interrupts_x86();

    dojo_check_video_mode();

    dojo_set_theme(THEME_UGLYDOJO);
    dojo_clear_screen();

    printk("Welcome to the Dojo!\n");
    printk("---> Using VGA text mode \n");

    while (1);
}

