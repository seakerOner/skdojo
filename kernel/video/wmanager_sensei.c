#include "./wmanager_sensei.h"

WManagerSensei wmanager_sensei;

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
        printk("[KERNEL] Maximum windows reached!\n");
        return -1;
    }
    u32 id = wmanager_sensei.window_count++;
    DojoWindow* window = &wmanager_sensei.windows[id];

    window->start_row = row;
    window->start_col = col;
    window->width     = width;
    window->height    = height;

    sensei->driver.new_framebuffer(row, col, width, height, &window->fb_stub);

    window->framebuffer = &window->fb_stub;
    return id;
}

DojoWindow* wmanager_get_focused() {
    return &wmanager_sensei.windows[wmanager_sensei.focused];
}

void wmanager_focus(u32 id) {
    wmanager_sensei.focused = id;
}

// TODO:
void compositor_render();
