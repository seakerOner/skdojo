#ifndef COMPOSITOR_SENSEI_H
#define COMPOSITOR_SENSEI_H

#include "../inttype.h"
#include "../printk/printk.h"
#include "video_sensei.h"

#define MAX_WINDOWS 10

typedef struct {
    void* framebuffer;
    u32 start_row;
    u32 start_col;
    u32 width;
    u32 height;
} DojoWindow;

typedef struct {
    DojoWindow windows[MAX_WINDOWS];
    u32 window_count;
    u32 focused;
} CompositorSensei;

void create_compositor_sensei();

CompositorSensei* get_compositor_sensei();

i32 compositor_create_window(u32 row, u32 col, u32 width, u32 height, VideoSensei* sensei);

DojoWindow* compositor_get_focused();
void compositor_focus(u32 id);

void compositor_render();

#endif
