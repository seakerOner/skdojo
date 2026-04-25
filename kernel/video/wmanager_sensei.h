#ifndef WMANAGER_SENSEI_H
#define WMANAGER_SENSEI_H

#include "../inttype.h"
#include "../printk/printk.h"
#include "video_sensei.h"

#define MAX_WINDOWS 11

// TODO: Change fb_stub for dynamic memory when mem model is done

typedef struct {
    u32 id;
    u32 start_width;
    u32 start_heigth;
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

i32 wmanager_create_window( u32 row, u32 col, u32 width, u32 height, VideoSensei* sensei );

DojoWindow* wmanager_get_focused();
DojoWindow* wmanager_get_window( u32 id );
void wmanager_focus( u32 id );
void dojo_clear_screen( DojoWindow* window );

void wmanager_render();

#endif
