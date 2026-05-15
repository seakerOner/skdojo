#include "./bios_boot_info.h"

#include "event_router/event_router.h"
#include "interrupts/k_interrupts.h"
#include "inttype.h"
#include "keyboard/keyboard_sensei.h"
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

    printk_init();

    keyboard_sensei_init();
    start_time_sensei();
    create_processes_sensei();
    VideoSensei* sensei_v = create_video_sensei();

    // everything below this line must be modularized

    DojoProcessSpawnConfig tatami_cfg = {
        .type  = NATIVE_PROC,
        .entry = tatami_main,
        .name  = "tatami",
    };

    DojoProcess* p_tatami = process_spawn( &tatami_cfg );

    init_event_router( p_tatami->pid );

    while ( 1 ) {
        event_router_poll();

        // TODO: upgrade to a scheduler 
        processes_update();
    }
}

