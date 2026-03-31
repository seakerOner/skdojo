
#include "./bios_boot_info.h"

#include "interrupts/k_interrupts.h"
#include "themes/themes.h"

#include "video/video_sensei.h"
#include "video/wmanager_sensei.h"
#include "video/compositor_sensei.h"
#include "memory/memory_sensei.h"
#include "memory/kheap.h"

#include "terminal/terminal.h"
#include "printk/printk.h"


void kmain(BiosBootInfo* boot_info) {
    init_interrupts_x86();


    MemorySensei* sensei_mem = create_memory_sensei(boot_info);
    start_kheap(sensei_mem); // default 14MB virtual memory

    VideoSensei* sensei_v = create_video_sensei();

    dojo_set_theme(THEME_DARKMODE);

    create_wmanager_sensei();
    WManagerSensei* sensei_wmanager = get_wmanager_sensei();

    i32 root_window_id = wmanager_create_window(0, 0, 
                                    sensei_v->screen_width, 
                                    sensei_v->screen_height, sensei_v);
    wmanager_focus(root_window_id);

    DojoWindow* root_window =  wmanager_get_window(root_window_id);
    dojo_clear_screen(root_window);

    CompositorSensei* comp_sensei = create_compositor_sensei(root_window);

    CompWinFrame* root_win_frame = compositor_create_window_current_row(comp_sensei);
    CompWinFrame* second_win_frame = compositor_create_window_current_row(comp_sensei);
    // CompWinFrame* third_win_frame = compositor_create_window_current_row(comp_sensei);
    // compositor_create_window_new_row(comp_sensei);

    
    DojoTerminal root_terminal = {0};
    terminal_new(root_win_frame, &root_terminal);
    DojoTerminal second_terminal = {0};
    terminal_new(second_win_frame, &second_terminal);
    // DojoTerminal third_terminal = terminal_new(third_win_frame);

    u32 root_t_comp_tag          = root_terminal.frame->id;
    u32 scnd_t_comp_tag = second_terminal.frame->id;
    // u32 third_t_comp_tag = third_terminal.frame->id;
    
    compositor_focus_frame(comp_sensei, root_t_comp_tag);

    terminal_print(&root_terminal, "Welcome to the Dojo!\nContact: seakerone@proton.me\n\n");
    terminal_putc(&root_terminal, '>');
    
    terminal_print(&second_terminal, "Using VGA text mode \n");
    terminal_print(&second_terminal, ">PHYSICAL RAM STATS:\n");
    terminal_print(&second_terminal, "- USABLE MEMORY:   ~");
    terminal_printDEC(&second_terminal, sensei_mem->physical_stats.bytes_usable/MB(1));
    terminal_print(&second_terminal, "MB\n");
    terminal_print(&second_terminal, "- RESERVED MEMORY: ~");
    terminal_printDEC(&second_terminal, sensei_mem->physical_stats.bytes_reserved/KB(1));
    terminal_print(&second_terminal, "KB\n");
    terminal_print(&second_terminal, "- BAD MEMORY:      ~");
    terminal_printDEC(&second_terminal, sensei_mem->physical_stats.bytes_bad_mem/KB(1));
    terminal_print(&second_terminal, "KB\n");

    terminal_print(&second_terminal, ">KERNEL MEM STATS:\n");
    terminal_print(&second_terminal, "- HEAP CAPACITY:   ~");
    terminal_printDEC(&second_terminal, sensei_mem->kernel_info.heap_bytes_cap/MB(1));
    terminal_print(&second_terminal, "MB\n");
    terminal_print(&second_terminal, "- HEAP FREE:       ~");
    terminal_printDEC(&second_terminal, sensei_mem->kernel_info.heap_bytes_free/MB(1));
    terminal_print(&second_terminal, "MB\n");
    terminal_print(&second_terminal, "- HEAP USED:       ~");
    terminal_printDEC(&second_terminal, sensei_mem->kernel_info.heap_bytes_used/KB(1));
    terminal_print(&second_terminal, "KB\n");
    terminal_print(&second_terminal, "- PAGES HANGED:     ");
    terminal_printDEC(&second_terminal, sensei_mem->kernel_info.heap_pages_hanged);

    terminal_print(&second_terminal, "\n>Senseis activated:\n"
            "- Memory Sensei\n"
            "- Video Sensei\n"
            "- Window Manager Sensei\n"
            "- Compositor Sensei\n"
            "- Keyboard Sensei\n"
            "\n");
    terminal_putc(&second_terminal, '>');

    while (1) {
        while(keyboard_has_events()) {
            // TODO: use window id to get focused window and find the 
            // corresponding compositor and its focused frame and poll 
            // everything
            //
            //u32 f_window_id = wmanager_get_focused()->id;

            KeyEvent ev;
            if (!keyboard_pop_event(&ev)) {
                continue;
            }

            if (compositor_poll(comp_sensei, &ev))
                continue;

            u32 focused_frame = comp_sensei->focused_node.frame_id;

            if (root_t_comp_tag == focused_frame) 
                terminal_poll(&root_terminal, &ev);
            if (scnd_t_comp_tag == focused_frame)
                terminal_poll(&second_terminal, &ev);
            // if (third_t_comp_tag == focused_frame)
            //     terminal_poll(&third_terminal, &ev);
        }
    }
}

