#include "processes_sensei.h"

ProcessesSensei processes_sensei = {0};

void create_processes_sensei() {
    processes_sensei.capacity = MAX_PROCESSES;
    processes_sensei.count    = 0;
}

DojoProcess* processes_sensei_new_handle() {
    if (processes_sensei.count >= processes_sensei.capacity) {
        return NULL;
    }
    return &processes_sensei.processes[processes_sensei.count++];
}

void processes_update() {
    for (u64 x = 0; x < processes_sensei.count; x++) {
        if (processes_sensei.processes[x].on_update)
            processes_sensei.processes[x].on_update(processes_sensei.processes[x].app_data);
    }
}
