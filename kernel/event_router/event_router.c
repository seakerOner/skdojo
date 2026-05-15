#include "event_router.h"

static EventRouter e_router = {0};

void init_event_router( u64 tatami_pid ) {
    e_router.proc_sensei = get_processes_sensei();
    e_router.tatami_pid  = tatami_pid;
};

EventRouter* get_event_router() {
    return &e_router;
};

void event_router_dispatch_event( DojoEvent* ev ) {
    EventRouter* r = &e_router;

    switch ( ev->type ) {
        case DJ_EVENT_KEYBOARD: 
        case DJ_EVENT_WINDOW_CLOSE:
        case DJ_EVENT_WINDOW_RESIZE: {
            DojoProcess* p = 
                processes_get( r->tatami_pid );
            
            if ( p )
                dojo_process_push_event(p, ev);

            break;
        }
        case DJ_EVENT_PROCESS_MESSAGE: {
            DojoProcess* p =
                processes_get( ev->message.to_pid );
            if ( p )
                dojo_process_push_event(p, ev);

            break;
        }
        default:
            break;
    }
}

void event_router_poll() {
    while( keyboard_has_events() ) {
        KeyEvent key_ev;
        if ( !keyboard_pop_event( &key_ev ) ) 
            continue;

        DojoEvent ev = {
            .type       = DJ_EVENT_KEYBOARD,
            .keyboard   = key_ev,
        };

        EventRouter* er = get_event_router();
        
        event_router_dispatch_event( &ev );
    }
}
