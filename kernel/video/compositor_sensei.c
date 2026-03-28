#include "./compositor_sensei.h"
#include "video_sensei.h"
#include "wmanager_sensei.h"

CompositorSensei compositor_sensei = {0};

CompositorSensei* create_compositor_sensei(DojoWindow* window) {

    compositor_sensei.window = window;
    compositor_sensei.max_frames                = MAX_WINDOWS;
    compositor_sensei.focused_window            = &wmanager_get_focused()->id;
    compositor_sensei.grid.curr_max_cols        = 0;
    compositor_sensei.grid.curr_max_rows        = 0;
    compositor_sensei.focused_node.row          = 0;
    compositor_sensei.focused_node.col          = 0;
    compositor_sensei.focused_node.col_segment  = 0;

    for (u32 x = 0; x < MAX_WINDOWS; x++) {
        CompWinBorder* border = &compositor_sensei.win_border[x];

        border->hor_border_c = '-';
        border->ver_border_c = '|';

        compositor_sensei.grid.curr_ids_in_col[x] = 0;
    }


    return &compositor_sensei;
}

// returns NULL if window is not created
CompWinFrame* compositor_create_window_current_row(CompositorSensei* c_sensei) {
    if (c_sensei->grid.curr_max_cols >=  c_sensei->max_frames ||
            c_sensei->grid.curr_max_rows >= c_sensei->max_frames)
        return NULL;
    if (c_sensei->frame_count >= c_sensei->max_frames)
        return NULL;

    if (!compositor_sensei.grid.curr_max_rows)
        compositor_sensei.grid.curr_max_rows = 1;

    c_sensei->grid.curr_max_cols++;
    c_sensei->grid.curr_ids_in_col[c_sensei->focused_node.col]++;
    c_sensei->frame_count++;

    u32 frame_id = c_sensei->frame_count;

    CompWinFrame* frame   = &c_sensei->win_frame[frame_id];
    frame->id             = frame_id;
    frame->parent_window  = c_sensei->window;
    
    // u32 grid_row = c_sensei->focused_node.row;
    // u32 grid_col = c_sensei->focused_node.col;
    u32 grid_row = c_sensei->grid.curr_max_rows - 1;
    u32 grid_col = c_sensei->grid.curr_max_cols - 1;
    u32 grid_seg = c_sensei->grid.curr_ids_in_col[c_sensei->focused_node.col] - 1;
    c_sensei->focused_node.col_segment = grid_seg;
   
    CompGridNode* node = &c_sensei->nodes[frame_id];

    node->id = c_sensei->grid.curr_ids_in_col[c_sensei->focused_node.col] - 1;
    node->row = grid_row;
    node->col = grid_col;
    node->resize_offsets.down  = 0;
    node->resize_offsets.up    = 0;
    node->resize_offsets.left  = 0;
    node->resize_offsets.right = 0;
    c_sensei->grid.data[grid_row][grid_col][grid_seg] = frame_id;


    comp_update_grid(c_sensei);

    return frame;
};

CompWinFrame* compositor_create_window_new_row(CompositorSensei* c_sensei);

void compositor_focus_frame(CompositorSensei* c_sensei, u32 frame_id) {
    if (frame_id >= c_sensei->max_frames)
        return;


    for (u32 x = 1; x <= c_sensei->max_frames; x++) {
        CompGridNode* node = &c_sensei->nodes[x];
        if (node->id == frame_id) {
            FocusedNode* f_node = &c_sensei->focused_node;
            
            f_node->frame_id    = frame_id;
            f_node->row         = node->row;
            f_node->col         = node->col;
            f_node->col_segment = node->id;
        }
    }
}

void comp_update_grid(CompositorSensei* c_sensei) {
    u32 base_height = c_sensei->window->height / c_sensei->grid.curr_max_rows ;
    u32 base_width  = c_sensei->window->width / c_sensei->grid.curr_max_cols ;

    for (u32 f = 1; f <= c_sensei->frame_count; f++) {
        CompGridNode* node = &c_sensei->nodes[f];
        //
        // cols
        //

        u32 abs_id = (u32)c_sensei->grid.data[node->row][node->col][node->id];

 
        u32 row_todraw     = base_height * node->row;
        u32 abs_height     = base_height;

        u32 col_todraw     = base_width * node->col;
        u32 abs_width      = base_width;

        CompWinBorder* border = &c_sensei->win_border[abs_id];
        border->start_width   = col_todraw;
        border->start_height  = row_todraw;
        border->height        = abs_height;
        border->width         = abs_width;

        CompWinFrame* frame   = &c_sensei->win_frame[abs_id];
        frame->start_width    = col_todraw + 1;
        frame->start_height   = row_todraw + 1;
        frame->width          = abs_width - 2;
        frame->height         = abs_height - 2;
    }
}


u32 compositor_poll(CompositorSensei* c_sensei, KeyEvent* ev) {
    if (ev->pressed && ev->key == KEY_1 && ev->shift && ev->super) {
        compositor_focus_frame(c_sensei, 1);
        return 1;
    }
    if (ev->pressed && ev->key == KEY_2 && ev->shift && ev->super) {
        compositor_focus_frame(c_sensei, 2);
        return 1;
    }
    if (ev->pressed && ev->key == KEY_3 && ev->shift && ev->super) {
        compositor_focus_frame(c_sensei, 3);
        return 1;
    }
    if (ev->pressed && ev->key == KEY_4 && ev->shift && ev->super) {
        compositor_focus_frame(c_sensei, 4);
        return 1;
    }
    if (ev->pressed && ev->key == KEY_5 && ev->shift && ev->super) {
        compositor_focus_frame(c_sensei, 5);
        return 1;
    }
    if (ev->pressed && ev->key == KEY_6 && ev->shift && ev->super) {
        compositor_focus_frame(c_sensei, 6);
        return 1;
    }
    if (ev->pressed && ev->key == KEY_7 && ev->shift && ev->super) {
        compositor_focus_frame(c_sensei, 7);
        return 1;
    }
    if (ev->pressed && ev->key == KEY_8 && ev->shift && ev->super) {
        compositor_focus_frame(c_sensei, 8);
        return 1;
    }
    if (ev->pressed && ev->key == KEY_9 && ev->shift && ev->super) {
        compositor_focus_frame(c_sensei, 9);
        return 1;
    }

    return 0;
}

void comp_draw_cell(CompWinFrame* frame, u32 row, u32 col, char c, StyleColor style) {
    get_video_sensei()->driver.draw_cell(frame->parent_window->framebuffer, 
                                                row + frame->start_height,
                                                col + frame->start_width,
                                                c, style);
};

void comp_read_cell(CompWinFrame* frame, u32 row, u32 col, char* out, StyleColor* style_out) {
    get_video_sensei()->driver.read_cell(frame->parent_window->framebuffer,
                                                row + frame->start_height,
                                                col + frame->start_width,
                                                out, style_out);
};
void comp_clear(CompWinFrame* frame, const StyleColor colors) {
    for (u32 r = 0; r < frame->height; r++) {
        for (u32 c = 0; c < frame->width; c++) {
            get_video_sensei()->driver.draw_cell(frame->parent_window->framebuffer,
                                                r + frame->start_height,
                                                c + frame->start_width,
                                                ' ',
                                                colors);
        }
    }
};
