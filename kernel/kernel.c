#include "interrupts/k_interrupts.h"
#include "themes/themes.h"

#include "video/video_sensei.h"
#include "video/compositor_sensei.h"

#include "terminal/terminal.h"
#include "printk/printk.h"

// TODO: (Memory Managment)
//   Use the e820 RAM information from bootloader to know how many usable memory to page

void kmain() {
    init_interrupts_x86();

    create_video_sensei();
    VideoSensei* sensei_v = get_video_sensei();

    create_compositor_sensei();
    u32 root_window_id = compositor_create_window(0, 0, 
                                    sensei_v->screen_width, 
                                    sensei_v->screen_height, sensei_v);

    compositor_focus(root_window_id);

    // new_terminal(sensei_v->screen_height, sensei_v->screen_width);

    dojo_set_theme(THEME_UGLYDOJO);

    dojo_clear_screen();


    printk("Welcome to the Dojo!\n");
    printk("---> Using VGA text mode \n");

    while (1);
}

