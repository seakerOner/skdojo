#include "tatami.h"

#include "../event_router/event_router.h"

static DojoTatami dojotatami = {0};

DojoTatami* tatami_start( u64 pid ) {
    VideoSensei* sensei_v    = get_video_sensei();
    MemorySensei* sensei_mem = get_mem_sensei();

    dojotatami.own_pid = pid;

    create_wmanager_sensei();

    i32 root_window_id = wmanager_create_window( 0, 0, 
            sensei_v->screen_width, 
            sensei_v->screen_height, sensei_v );

    wmanager_focus( root_window_id );

    DojoWindow* root_window =  wmanager_get_window( root_window_id );
    dojo_clear_screen( root_window );

    CompositorSensei* comp_sensei = create_compositor_sensei( root_window );
    dojotatami.cmp_sensei = comp_sensei;
    return &dojotatami;
}

boolean tatami_poll_global_shortcuts( DojoProcess* self, CompositorSensei* c_sensei, KeyEvent* ev ) {
    // create a new frame on current row
    if ( ev->pressed && ev->key == KEY_ENTER && ev->shift && ev->super ) {
        CompWinFrame* f = compositor_create_frame_current_row( c_sensei );

        // create a new terminal
        DojoProcessSpawnConfig cfg = {0};
        terminal_new_proc_cfg( &cfg );
        DojoProcess* t_proc = process_spawn(&cfg);

        DojoEvent msg = {
            .type = DJ_EVENT_PROCESS_MESSAGE,
            .message = {
                .from_pid = self->pid,
                // TODO: with different PML4 we need a shared memory resource
                .data     = ( void * )f,
            }
        };

        dojo_process_push_event(t_proc, &msg);

        return 1;
    }
    // create frame on new row
    if ( ev->pressed && ev->key == KEY_N && ev->shift && ev->super ) {
        CompWinFrame* f = compositor_create_frame_new_row( c_sensei );

        // create a new terminal
        DojoProcessSpawnConfig cfg = {0};
        terminal_new_proc_cfg( &cfg );
        DojoProcess* t_proc = process_spawn(&cfg);

        DojoEvent msg = {
            .type = DJ_EVENT_PROCESS_MESSAGE,
            .message = {
                .from_pid = self->pid,
                // TODO: with different PML4 we need a shared memory resource
                .data     = ( void * )f,
            }
        };

        dojo_process_push_event(t_proc, &msg);

        return 1;
    }
    // delete focused frame
    if ( ev->pressed && ev->key == KEY_Q && ev->shift && ev->super ) {
        compositor_destroy_focused_frame( c_sensei );
        return 1;
    }

    //
    // move focused frame
    //
    if ( ev->pressed && ev->key == KEY_H && ev->shift && ev->super ) {
        compositor_focus_left( c_sensei );
        return 1;
    }
    if ( ev->pressed && ev->key == KEY_J && ev->shift && ev->super ) {
        compositor_focus_down( c_sensei );
        return 1;
    }
    if ( ev->pressed && ev->key == KEY_K && ev->shift && ev->super ) {
        compositor_focus_up( c_sensei );
        return 1;
    }
    if ( ev->pressed && ev->key == KEY_L && ev->shift && ev->super ) {
        compositor_focus_right( c_sensei );
        return 1;
    }

    return 0;

}

void tatami_handle_event( DojoProcess* self, DojoEvent* ev ) {
    // TODO: use window id to get focused window and find the 
    // corresponding compositor and its focused frame and poll 
    // everything
    //
    //u32 f_window_id = wmanager_get_focused()->id;
    
    CompositorSensei* cmp_sensei = dojotatami.cmp_sensei;

    switch ( ev->type ) {
        case DJ_EVENT_KEYBOARD: {
            KeyEvent key_ev = ev->keyboard;
            if ( tatami_poll_global_shortcuts( self , cmp_sensei, &key_ev ) )
                return;

            CompWinFrame* focused_frame = compositor_get_focused_frame( cmp_sensei );

            dojo_process_push_event(focused_frame->process, ev);

            break;
        };
        default:
            break;
    }
}

void tatami_main( DojoProcess* self ) {
    static boolean initialized = FALSE;

    if ( !initialized ) {
        tatami_start( self->pid );

        dojo_set_theme( THEME_DARKMODE );
        CompWinFrame* root_win_frame   = compositor_create_frame_current_row( dojotatami.cmp_sensei );

        DojoProcessSpawnConfig term_cfg;
        terminal_new_proc_cfg( &term_cfg );
        DojoProcess* term_proc =  process_spawn( &term_cfg );

        DojoEvent msg = {
            .type = DJ_EVENT_PROCESS_MESSAGE,
            .message = {
                .from_pid = self->pid,
                .to_pid   = term_proc->pid,
                // TODO: with different PML4 we need a shared memory resource
                .data     = ( void * )root_win_frame,
            }
        };

        event_router_dispatch_event( &msg );

        //dojo_process_push_event( term_proc, &msg );

        compositor_focus_frame( dojotatami.cmp_sensei, 0 );

        // terminal_print( &root_terminal, KSTR( "Welcome to the Dojo!\n>Using Tatami\nContact: seakerone@proton.me\n\n"  ));
        // terminal_putc( &root_terminal, '>' );

        initialized = TRUE;

    }


    // while (1) {
        DojoEvent ev;

        while ( dojo_process_pop_event( self, &ev ) ) 
            tatami_handle_event( self, &ev );

        // TODO:
        // scheduler_yield();
    // }
}
