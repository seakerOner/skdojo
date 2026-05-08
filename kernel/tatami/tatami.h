#ifndef TATAMI_H
#define TATAMI_H

#include "../memory/memory_sensei.h"
#include "../terminal/terminal.h"
#include "../video/wmanager_sensei.h"
#include "../video/video_sensei.h"
#include "../video/compositor_sensei.h"
#include "../keyboard/keyboard_sensei.h"
#include "../process/process.h"

typedef struct {
    u64 own_pid;
    CompositorSensei* cmp_sensei;
} DojoTatami;

void tatami_main( DojoProcess* proc );

DojoTatami* tatami_start( u64 pid );

void tatami_poll(); 

#endif
