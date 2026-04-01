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

DojoProcess* processes_sensei_new_handle();

void processes_update();

#endif
