#include "processes_sensei.h"

static ProcessesSensei processes_sensei = {0};

void create_processes_sensei() {
    processes_sensei.capacity = MAX_PROCESSES;
    processes_sensei.count    = 0;
}

DojoProcess* processes_sensei_new_handle() {
    if ( processes_sensei.count >= processes_sensei.capacity ) {
        return NULL;
    }
    DojoProcess* proc = &processes_sensei.processes[processes_sensei.count];
    proc->pid = processes_sensei.count++;

    return proc;
}

void processes_update() {
    for ( u64 x = 0; x < processes_sensei.count; x++ ) {
        if ( processes_sensei.processes[x].on_update )
            processes_sensei.processes[x].on_update( processes_sensei.processes[x].app_data );
    }
}
