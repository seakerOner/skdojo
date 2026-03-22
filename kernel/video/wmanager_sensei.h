#ifndef COMPOSITOR_SENSEI_H
#define COMPOSITOR_SENSEI_H

#include "../inttype.h"
#include "../printk/printk.h"
#include "video_sensei.h"

#define MAX_WINDOWS 10

// TODO: Change fb_stub for dynamic memory when mem model is done

typedef struct {
    u32 start_row;
    u32 start_col;
    u32 width;
    u32 height;
    FrameBufferHandle framebuffer;
    FrameBuffer fb_stub;
} DojoWindow;

typedef struct {
    DojoWindow windows[MAX_WINDOWS];
    u32 window_count;
    u32 focused;
} WManagerSensei;

void create_wmanager_sensei();

WManagerSensei* get_wmanager_sensei();

i32 wmanager_create_window(u32 row, u32 col, u32 width, u32 height, VideoSensei* sensei);

DojoWindow* wmanager_get_focused();
void wmanager_focus(u32 id);

void wmanager_render();

#endif
