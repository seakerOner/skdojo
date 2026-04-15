#include "./wmanager_sensei.h"

static WManagerSensei wmanager_sensei;

void create_wmanager_sensei() {
    wmanager_sensei.focused = 0;
    wmanager_sensei.window_count = 0;
}

WManagerSensei* get_wmanager_sensei() {
    return &wmanager_sensei;
}

// -1 if error
i32 wmanager_create_window(u32 row, u32 col, u32 width, u32 height, VideoSensei* sensei) {
    if (wmanager_sensei.window_count >= MAX_WINDOWS) {
        printk_warn("[KERNEL] Maximum windows reached!\n");
        return -1;
    }
    u32 id = wmanager_sensei.window_count++;
    DojoWindow* window = &wmanager_sensei.windows[id];

    window->id              = id;
    window->start_heigth    = row;
    window->start_width     = col;
    window->width           = width;
    window->height          = height;

    sensei->driver.new_framebuffer(row, col, width, height, &window->fb_stub);

    window->framebuffer = &window->fb_stub;
    return id;
}

DojoWindow* wmanager_get_focused() {
    return &wmanager_sensei.windows[wmanager_sensei.focused];
}

DojoWindow* wmanager_get_window(u32 id) {
    if (id >= MAX_WINDOWS) {
        printk_warn("[Window Sensei ERROR] Out of bounds get_window()\n");
        return NULL;
    }
    return &wmanager_sensei.windows[id];
}

void wmanager_focus(u32 id) {
    wmanager_sensei.focused = id;
}

void dojo_clear_screen(DojoWindow* window) {
    VideoSensei* sensei = get_video_sensei();
    sensei->driver.clear(window->framebuffer, dojo_get_theme()->palette.main_colors);
}

// TODO:
void compositor_render();
