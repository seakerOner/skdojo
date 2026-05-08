#include "processes_sensei.h"

static ProcessesSensei processes_sensei = {0};

void create_processes_sensei() {
    processes_sensei.capacity = MAX_PROCESSES;
    processes_sensei.count    = 0;
}

ProcessesSensei* get_processes_sensei() {
    return &processes_sensei;
}

DojoProcess* processes_get(u64 pid) {
    if ( pid >= processes_sensei.count )
        return NULL;

    return &processes_sensei.processes[pid];
}

DojoProcess* processes_sensei_new_handle() {
    if ( processes_sensei.count >= processes_sensei.capacity ) {
        return NULL;
    }
    DojoProcess* proc = &processes_sensei.processes[processes_sensei.count];
    proc->pid = processes_sensei.count++;

    return proc;
}

DojoProcess* process_spawn( DojoProcessSpawnConfig* cfg ) {
    DojoProcess* proc = processes_sensei_new_handle();
    if ( proc == NULL )
        return NULL;

    proc->type       = cfg->type;
    proc->entry      = cfg->entry;

    return proc;
}

void processes_update() {
    for ( u64 x = 0; x < processes_sensei.count; x++ ) {
        DojoProcess* proc = &processes_sensei.processes[x];

        if ( proc->state != PROC_RUNNING )
            continue;

        proc->entry( proc );
    }
}
