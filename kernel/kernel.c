#include "./bios_boot_info.h"

#include "interrupts/k_interrupts.h"
#include "inttype.h"
#include "memory/kata.h"
#include "terminal/terminal.h"
#include "themes/themes.h"

#include "video/video_sensei.h"
#include "memory/memory_sensei.h"
#include "memory/kheap.h"
#include "process/processes_sensei.h"
#include "time/time_sensei.h"

#include "tatami/tatami.h"
#include "printk/printk.h"

void kmain( BiosBootInfo* boot_info ) {
    MemorySensei* sensei_mem = create_memory_sensei( boot_info );
    kata_populate_regions( boot_info );  // ^^ populate kata in mem sensei
    start_kheap( sensei_mem );

    init_interrupts_x86();

    start_time_sensei();
    create_processes_sensei();
    VideoSensei* sensei_v = create_video_sensei();

    // everything below this line must be modularized

    dojo_set_theme( THEME_DARKMODE );
    DojoTatami* tatami = tatami_start();
    CompWinFrame* root_win_frame = compositor_create_frame_current_row( tatami->cmp_sensei );
    CompWinFrame* second_win_frame = compositor_create_frame_current_row( tatami->cmp_sensei );


    if ( !root_win_frame || !second_win_frame )
        while (1);  // hang

    DojoTerminal root_terminal = {0};
    terminal_new( root_win_frame, &root_terminal );
    DojoTerminal second_terminal = {0};
    terminal_new( second_win_frame, &second_terminal );

    u32 root_t_comp_tag          = root_terminal.frame->id;
    compositor_focus_frame( tatami->cmp_sensei, root_t_comp_tag );

    terminal_print( &root_terminal, "Welcome to the Dojo!\n>Using Tatami\nContact: seakerone@proton.me\n\n" );
    terminal_putc( &root_terminal, '>' );

    terminal_print( &second_terminal, "Using VGA text mode \n" );
    terminal_print( &second_terminal, ">PHYSICAL RAM STATS:\n" );
    terminal_print( &second_terminal, "- [USABLE] ~" );
    terminal_printDEC( &second_terminal, sensei_mem->physical_stats.bytes_usable/MB(1) );
    terminal_print( &second_terminal, "MB" );
    terminal_print( &second_terminal, " [RESERVED] ~" );
    terminal_printDEC( &second_terminal, sensei_mem->physical_stats.bytes_reserved/KB(1) );
    terminal_print( &second_terminal, "KB\n" );
    terminal_print( &second_terminal, "- [BAD]    ~" );
    terminal_printDEC( &second_terminal, sensei_mem->physical_stats.bytes_bad_mem/KB(1) );
    terminal_print( &second_terminal, "KB\n" );

    terminal_print( &second_terminal, ">KERNEL HEAP MEM STATS:\n" );
    terminal_print( &second_terminal, "- [CAPACITY] ~" );
    terminal_printDEC( &second_terminal, sensei_mem->kernel_info.heap_bytes_cap/MB(1) );
    terminal_print( &second_terminal, "MB" );
    terminal_print( &second_terminal, " [FREE] ~" );
    terminal_printDEC( &second_terminal, sensei_mem->kernel_info.heap_bytes_free/MB(1) );
    terminal_print( &second_terminal, "MB\n" );
    terminal_print( &second_terminal, "- [USED]     ~" );
    terminal_printDEC( &second_terminal, sensei_mem->kernel_info.heap_bytes_used/KB(1) );
    terminal_print( &second_terminal, "KB\n" );
    terminal_print( &second_terminal, "- PAGES HANGED:     " );
    terminal_printDEC( &second_terminal, sensei_mem->kernel_info.heap_pages_hanged );


    terminal_print( &second_terminal,"\n>Senseis activated:\n"
            "- Memory Sensei\n"
            "- Video Sensei\n"
            "- Window Manager Sensei\n"
            "- Compositor Sensei\n"
            "- Keyboard Sensei\n"
            "- Processes Sensei\n"
            "- Time Sensei\n"
            "\n" );
    terminal_putc( &second_terminal, '>' );

    // TODO: second console is crashing when adding to history
    while ( 1 ) {
        tatami_poll( tatami->cmp_sensei );

        // TODO: upgrade to a scheduler 
        processes_update();
    }
}

