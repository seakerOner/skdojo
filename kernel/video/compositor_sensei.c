#include "./compositor_sensei.h"

CompositorSensei compositor_sensei;

void create_compositor_sensei() {
    compositor_sensei.focused = 0;
    compositor_sensei.window_count = 0;
}

CompositorSensei* get_compositor_sensei() {
    return &compositor_sensei;
}

// -1 if error
i32 compositor_create_window(u32 row, u32 col, u32 width, u32 height, VideoSensei* sensei) {
    if (compositor_sensei.window_count >= MAX_WINDOWS) {
        printk("[KERNEL] Maximum windows reached!\n");
        return -1;
    }
    u32 id = compositor_sensei.window_count++;
    DojoWindow* window = &compositor_sensei.windows[id];

    window->start_row = row;
    window->start_col = col;
    window->width     = width;
    window->height    = height;

    sensei->driver.new_framebuffer(row, col, width, height, window->framebuffer);

    return id;
}

DojoWindow* compositor_get_focused() {
    return &compositor_sensei.windows[compositor_sensei.focused];
}

void compositor_focus(u32 id) {
    compositor_sensei.focused = id;
}

// TODO:
void compositor_render();
