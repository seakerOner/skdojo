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

boolean tatami_poll_global_shortcuts( CompositorSensei* c_sensei, KeyEvent* ev ) {
    // create a new frame on current row
    if ( ev->pressed && ev->key == KEY_ENTER && ev->shift && ev->super ) {
        compositor_create_frame_current_row( c_sensei );
        return 1;
    }
    // create frame on new row
    if ( ev->pressed && ev->key == KEY_N && ev->shift && ev->super ) {
        compositor_create_frame_new_row( c_sensei );
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
        compositor_focus_down(c_sensei);
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

    if (ev->type != DJ_EVENT_KEYBOARD)
        return;

    KeyEvent key_ev = ev->keyboard;
    CompositorSensei* cmp_sensei = dojotatami.cmp_sensei;
    
    if ( tatami_poll_global_shortcuts( cmp_sensei, &key_ev ) )
            return;
        
    CompWinFrame* focused_frame = compositor_get_focused_frame( cmp_sensei );

    dojo_process_push_event(focused_frame->process, ev);
}

void tatami_main( DojoProcess* self ) {
    tatami_start( self->pid );

    while (1) {
        DojoEvent ev;

        while ( dojo_process_pop_event( self, &ev ) ) 
            tatami_handle_event( self, &ev );

        // TODO:
        // scheduler_yield();
    }
}
