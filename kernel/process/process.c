#include "process.h"

boolean dojo_process_push_event(DojoProcess* process, DojoEvent* event) {
    if ( ( process->event_write - process->event_read ) >= MAX_PROCESS_EVENTS )
        return FALSE;

    u64 write_idx = process->event_write;
    u64 abs_idx = write_idx % MAX_PROCESS_EVENTS;

    COPY(event, (&process->events[abs_idx]) , sizeof( DojoEvent ));
    process->event_write++;

    return TRUE;
};

boolean dojo_process_pop_event(DojoProcess* process, DojoEvent* event) {
    if ( process->event_read >= process->event_write )
        return FALSE;

    u64 read_idx  = process->event_read  % MAX_PROCESS_EVENTS;

    DojoEvent* popd_ev = &process->events[read_idx];

    COPY( popd_ev, event, sizeof( DojoEvent ) );

    process->event_read++;

    return TRUE;
}
