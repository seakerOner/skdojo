#include "processes_sensei.h"
#include "../memory/kata.h"
#include "../printk/printk.h"

static ProcessesSensei processes_sensei = {0};

void create_processes_sensei() {
    processes_sensei.count    = 0;
    processes_sensei.capacity = MAX_PROCESSES;
    FILL(processes_sensei.processes, 0, ( sizeof( DojoProcess* ) * MAX_PROCESSES ) );
}

ProcessesSensei* get_processes_sensei() {
    return &processes_sensei;
}

DojoProcess* processes_get(u64 pid) {
    if ( pid >= processes_sensei.count )
        return NULL;

    return processes_sensei.processes[pid];
}

DojoProcess* processes_sensei_new_handle() {
    if ( processes_sensei.count >= processes_sensei.capacity ) {
        return NULL;
    }
    DojoProcess* ptr = ( DojoProcess* ) kata_alloc( ORDER_4KB, TRUE );

    if ( ptr == NULL ) {
        return NULL;
    }

    processes_sensei.processes[processes_sensei.count] = ptr;

    FILL(ptr, 0, sizeof ( DojoProcess ));
    ptr->pid = processes_sensei.count++;

    return ptr;
}

DojoProcess* process_spawn( DojoProcessSpawnConfig* cfg ) {
    DojoProcess* proc = processes_sensei_new_handle();
    if ( proc == NULL ) {
        printk_err( KSTR( "[PROC][SENSEI] Failed to allocate process struct\n" ) );
        return NULL;
    }

    proc->type       = cfg->type;
    proc->entry      = cfg->entry;
    COPY( cfg->name, proc->name, 20 );

    proc->state       = PROC_RUNNING;
    proc->event_read  = 0;
    proc->event_write = 0;

    return proc;
}

void processes_update() {
    for ( u64 x = 0; x < processes_sensei.count; x++ ) {
        DojoProcess* proc = processes_sensei.processes[x];

        if ( proc == NULL ) {
            printk_err( KSTR( "[PROC][UPDATE] Tried to run a process without a valid pointer access\n" ) );
            continue;
        }
        if ( proc->state != PROC_RUNNING )
            continue;

        if ( proc->entry )
            proc->entry( proc );
    }
}
