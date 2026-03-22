#include "interrupts/k_interrupts.h"
#include "themes/themes.h"

#include "video/video_sensei.h"
#include "video/wmanager_sensei.h"

#include "terminal/terminal.h"
#include "printk/printk.h"

// TODO: (Memory Managment)
//   Use the e820 RAM information from bootloader to know how many usable memory to page

void kmain() {
    init_interrupts_x86();

    create_video_sensei();
    VideoSensei* sensei_v = get_video_sensei();

    dojo_set_theme(THEME_UGLYDOJO);

    create_wmanager_sensei();
    WManagerSensei* sensei_wmanager = get_wmanager_sensei();

    u32 root_window_id = wmanager_create_window(0, 0, 
                                    sensei_v->screen_width/2 - 1, 
                                    sensei_v->screen_height, sensei_v);
    wmanager_focus(root_window_id);
    dojo_clear_screen();

    printk("---> Using VGA text mode \n");

    printk("Welcome to the Dojo!\n\n");
    printk("Senseis activated:\n"
            "-- Video Sensei\n"
            "-- Window Manager Sensei\n"
            "-- Keyboard Sensei\n"
            "\n");

    u32 second_id = wmanager_create_window(
                                     0,
                                     sensei_v->screen_width/2, 
                                     sensei_v->screen_width/2, 
                                     sensei_v->screen_height, sensei_v);
    wmanager_focus(second_id);
    dojo_clear_screen();

    DojoTerminal second_terminal = new_terminal(wmanager_get_window(second_id));

    wmanager_focus(root_window_id);
    DojoTerminal root_terminal = new_terminal(wmanager_get_window(root_window_id));

    while (1) {
        u32 focused_window = wmanager_get_focused()->id;

        if (root_terminal.window->id == focused_window)
            terminal_poll_events(&root_terminal);
        if (second_terminal.window->id == focused_window)
            terminal_poll_events(&second_terminal);
    }
}

