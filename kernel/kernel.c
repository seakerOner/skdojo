
#include "./bios_boot_info.h"

#include "interrupts/k_interrupts.h"
#include "themes/themes.h"

#include "video/video_sensei.h"
#include "video/wmanager_sensei.h"

#include "terminal/terminal.h"
#include "printk/printk.h"

#define CONFIG_PHYSICAL_START 0x100000

// TODO: (Memory Managment)
//   Use the e820 RAM information from bootloader to know how many usable memory to page

void kmain(BiosBootInfo* boot_info) {
    init_interrupts_x86();


    create_video_sensei();
    VideoSensei* sensei_v = get_video_sensei();

    dojo_set_theme(THEME_UGLYDOJO);

    create_wmanager_sensei();
    WManagerSensei* sensei_wmanager = get_wmanager_sensei();

    i32 root_window_id = wmanager_create_window(0, 0, 
                                    sensei_v->screen_width/2 - 1, 
                                    sensei_v->screen_height, sensei_v);
    wmanager_focus(root_window_id);
    dojo_clear_screen();


    i32 second_id = wmanager_create_window(
                                     0,
                                     sensei_v->screen_width/2, 
                                     sensei_v->screen_width/2, 
                                     sensei_v->screen_height, sensei_v);
    wmanager_focus(second_id);
    dojo_clear_screen();

    DojoTerminal second_terminal = terminal_new(wmanager_get_window(second_id));

    wmanager_focus(root_window_id);
    DojoTerminal root_terminal = terminal_new(wmanager_get_window(root_window_id));


    terminal_print(&root_terminal, "Welcome to the Dojo!\nContact: seakerone@proton.me\n\n");
    terminal_print(&root_terminal, "Using VGA text mode \n");
    u64 usable_ram_cap = 0;
    for(u64 x = 0; x < boot_info->boot_memmap_entries; x++) {
        if (boot_info->boot_memmap_addr[x].type == USABLE_RAM
                && boot_info->boot_memmap_addr[x].base_addr >= CONFIG_PHYSICAL_START) {
            usable_ram_cap += boot_info->boot_memmap_addr[x].length;
        }
    }
    terminal_print(&root_terminal, "USABLE RAM: ~");
    terminal_printDEC(&root_terminal, usable_ram_cap/(1024 * 1024));
    terminal_print(&root_terminal, "MB\n\n");

    terminal_print(&root_terminal, "Senseis activated:\n"
            "-- Video Sensei\n"
            "-- Window Manager Sensei\n"
            "-- Keyboard Sensei\n"
            "\n");
    terminal_putc(&root_terminal, '>');

    while (1) {
        u32 focused_window = wmanager_get_focused()->id;

        if (root_terminal.window->id == focused_window)
            terminal_poll_events(&root_terminal);
        if (second_terminal.window->id == focused_window)
            terminal_poll_events(&second_terminal);
    }
}

