#include "tatami.h"

static DojoTatami dojotatami = {0};

DojoTatami* tatami_start() {
    VideoSensei* sensei_v    = get_video_sensei();
    MemorySensei* sensei_mem = get_mem_sensei();

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

void tatami_poll( CompositorSensei* cmp_sensei ) {
    while( keyboard_has_events() ) {
            // TODO: use window id to get focused window and find the 
            // corresponding compositor and its focused frame and poll 
            // everything
            //
            //u32 f_window_id = wmanager_get_focused()->id;

            KeyEvent ev;
            if ( !keyboard_pop_event( &ev ) ) 
                continue;

            if ( compositor_poll( cmp_sensei, &ev ) )
                continue;

            CompWinFrame* focused_frame = compositor_get_focused_frame( cmp_sensei );

            if ( focused_frame->process && focused_frame->process->on_event )
                focused_frame->process->on_event( focused_frame->process->app_data, &ev );
    }
}
