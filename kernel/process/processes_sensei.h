#ifndef PROCESSES_SENSEI_H
#define PROCESSES_SENSEI_H

#include "process.h"

#define MAX_PROCESSES 16

typedef struct {
    DojoProcess processes[MAX_PROCESSES];
    u64 count;
    u64 capacity;
} ProcessesSensei;

void create_processes_sensei();

DojoProcess* process_spawn( DojoProcessSpawnConfig* cfg );

ProcessesSensei* get_processes_sensei();

DojoProcess* processes_sensei_new_handle();
DojoProcess* processes_get(u64 pid);

void processes_update();

#endif
