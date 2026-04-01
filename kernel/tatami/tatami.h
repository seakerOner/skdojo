#ifndef TATAMI_H
#define TATAMI_H

#include "../memory/memory_sensei.h"
#include "../terminal/terminal.h"
#include "../video/wmanager_sensei.h"
#include "../video/video_sensei.h"
#include "../video/compositor_sensei.h"
#include "../keyboard/keyboard_sensei.h"

typedef struct {
    CompositorSensei* cmp_sensei;
} DojoTatami;

DojoTatami* tatami_start(VideoSensei* v_sensei, MemorySensei* m_sensei); 
void tatami_poll(CompositorSensei* cmp_sensei); 

#endif
