#ifndef EVENTROUTER_H
#define EVENTROUTER_H

#include "../inttype.h"
#include "../process/process.h"
#include "../process/processes_sensei.h"
#include "../video/wmanager_sensei.h"
#include "../tatami/tatami.h"

typedef struct {
    ProcessesSensei* proc_sensei;
    u64 tatami_pid;
} EventRouter;

void init_event_router( u64 tatami_pid );

void event_router_poll();

EventRouter* get_event_router();
void event_router_dispatch_event( DojoEvent* ev );

#endif
