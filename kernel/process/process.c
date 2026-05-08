#include "process.h"

boolean dojo_process_push_event(DojoProcess* process, DojoEvent* event) {
    u64 write_idx = process->event_write++;
    u64 abs_idx = write_idx % MAX_PROCESS_EVENTS;
    
    COPY(event, &process->events[abs_idx], sizeof(DojoEvent));
    return TRUE;
};

boolean dojo_process_pop_event(DojoProcess* process, DojoEvent* event) {

    return TRUE;
}

void dojo_dispatch_event(DojoProcess* process, DojoEvent* event) {
    // switch (process->type) {
    //     case NATIVE_PROC:
    //         break;
    //     case FORTH_PROC:
    //         break;
    //     default:
    //         break;
    // }
    switch (event->type) {
        case DJ_EVENT_KEYBOARD:
            if (process->on_event)
                process->on_event( process->app_data, &event->keyboard );
            break;
        case DJ_EVENT_WINDOW_RESIZE:
            if (process->on_resize)
                process->on_resize( process->app_data, event->resize.width, event->resize.height );
            break;
        default:
            break;
    }

    // void ( *on_event   )( void*, KeyEvent* );
    // void ( *on_update  )( void* );
    // void ( *on_resize  )( void* app, u32 new_w, u32 new_h );
    // void ( *on_destroy )( void* );

}
