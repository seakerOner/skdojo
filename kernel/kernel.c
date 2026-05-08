#include "./bios_boot_info.h"

#include "event_router/event_router.h"
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

    DojoProcessSpawnConfig tatami_cfg = {
        .entry = tatami_main,
        .type  = NATIVE_PROC
    };
    dojo_set_theme( THEME_DARKMODE );

    DojoProcess* p_tatami = process_spawn(&tatami_cfg);

    init_event_router( p_tatami->pid );

    CompWinFrame* root_win_frame   = compositor_create_frame_current_row( tatami->cmp_sensei );
    CompWinFrame* second_win_frame = compositor_create_frame_current_row( tatami->cmp_sensei );

    if ( !root_win_frame || !second_win_frame )
        while (1);  // hang

    DojoTerminal root_terminal   = {0};
    terminal_new( root_win_frame, &root_terminal );
    DojoTerminal second_terminal = {0};
    terminal_new( second_win_frame, &second_terminal );

    u32 root_t_comp_tag          = root_terminal.frame->id;
    compositor_focus_frame( tatami->cmp_sensei, root_t_comp_tag );

    terminal_print( &root_terminal, KSTR( "Welcome to the Dojo!\n>Using Tatami\nContact: seakerone@proton.me\n\n"  ));
    terminal_putc( &root_terminal, '>' );

    while ( 1 ) {
        event_router_poll();

        // TODO: upgrade to a scheduler 
        processes_update();
    }
}

