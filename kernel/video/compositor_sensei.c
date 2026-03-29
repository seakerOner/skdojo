#include "./compositor_sensei.h"
#include "video_sensei.h"
#include "wmanager_sensei.h"

CompositorSensei compositor_sensei = {0};

CompositorSensei* create_compositor_sensei(DojoWindow* window) {

    compositor_sensei.window = window;
    compositor_sensei.max_frames                = MAX_WINDOWS;
    compositor_sensei.focused_window            = &wmanager_get_focused()->id;
    compositor_sensei.grid.curr_max_rows        = 0;
    compositor_sensei.focused_node.row          = 0;
    compositor_sensei.focused_node.col          = 0;
    compositor_sensei.focused_node.col_segment  = 0;

    for (u32 x = 0; x < MAX_WINDOWS; x++) {
        CompWinBorder* border = &compositor_sensei.win_border[x];

        border->hor_border_c    = 0xC4;
        border->ver_border_c    = 0xB3; 
        border->bot_border_c    = 0xC4;
        border->top_l_corner_c  = 0xDA;
        border->top_r_corner_c  = 0xBF;
        border->bot_l_corner_c  = 0xC0;
        border->bot_r_corner_c  = 0xD9;

        compositor_sensei.grid.curr_cols_in_row[x] = 0;
    }

    for (u32 r = 0; r < CS_GRID_ROWS; r++) {
        for (u32 c = 0; c < CS_GRID_COLS; c++) {
            compositor_sensei.grid.curr_layers[r][c] = 0;
        }
    }

    return &compositor_sensei;
}

static void comp_clean_borders(CompositorSensei* c_sensei); 
static void comp_draw_borders(CompositorSensei* c_sensei);

// returns NULL if window is not created
CompWinFrame* compositor_create_window_current_row(CompositorSensei* c_sensei) {
    if (c_sensei->grid.curr_cols_in_row[c_sensei->focused_node.row] >=  c_sensei->max_frames)
        return NULL;
    if (c_sensei->frame_count >= c_sensei->max_frames)
        return NULL;

    if (!c_sensei->grid.curr_max_rows)
        c_sensei->grid.curr_max_rows = 1;

    comp_clean_borders(c_sensei);

    u32 row = c_sensei->focused_node.row;
    c_sensei->grid.curr_cols_in_row[row]++;
    u32 col = c_sensei->grid.curr_cols_in_row[row] - 1;
    u32 layer = c_sensei->grid.curr_layers[row][col]++;
    c_sensei->frame_count++;

    u32 frame_id = c_sensei->frame_count;

    CompWinFrame* frame   = &c_sensei->win_frame[frame_id];
    frame->id             = frame_id;
    frame->parent_window  = c_sensei->window;
    
    u32 grid_row = row;
    u32 grid_col = col;
    u32 grid_seg = layer;
    c_sensei->focused_node.col_segment = grid_seg;
   
    CompGridNode* node = &c_sensei->nodes[frame_id];

    node->id                   = grid_seg;
    node->row                  = row;
    node->col                  = col;
    node->resize_offsets.down  = 0;
    node->resize_offsets.up    = 0;
    node->resize_offsets.left  = 0;
    node->resize_offsets.right = 0;
    c_sensei->grid.data[grid_row][grid_col][grid_seg] = frame_id;


    comp_update_grid(c_sensei);

    // draw all border after calculations
    comp_draw_borders(c_sensei);

    return frame;
};

CompWinFrame* compositor_create_window_new_row(CompositorSensei* c_sensei) {
    if (c_sensei->grid.curr_max_rows >=  c_sensei->max_frames)
        return NULL;
    if (c_sensei->frame_count >= c_sensei->max_frames)
        return NULL;

    comp_clean_borders(c_sensei);

    u32 new_row = c_sensei->grid.curr_max_rows++;
    u32 col = 0;

    c_sensei->grid.curr_cols_in_row[new_row] = 0;  // ensure 0 init
    c_sensei->grid.curr_cols_in_row[new_row]++;

    c_sensei->frame_count++;
    u32 frame_id = c_sensei->frame_count;


    CompWinFrame* frame = &c_sensei->win_frame[frame_id];
    frame->id = frame_id;
    frame->parent_window = c_sensei->window;

    u32 layer    = c_sensei->grid.curr_layers[new_row][col]++;
    u32 grid_seg = layer;

    c_sensei->focused_node.row         = new_row;
    c_sensei->focused_node.col         = col;
    c_sensei->focused_node.col_segment = layer;
    c_sensei->focused_node.frame_id    = frame_id;

    CompGridNode* node = &c_sensei->nodes[frame_id];

    node->id  = layer;
    node->row = new_row;
    node->col = col;

    node->resize_offsets.up     = 0;
    node->resize_offsets.down   = 0;
    node->resize_offsets.left   = 0;
    node->resize_offsets.right  = 0;

    c_sensei->grid.data[new_row][col][grid_seg] = frame_id;

    comp_update_grid(c_sensei);
    comp_draw_borders(c_sensei);

    return frame;
} ;

void compositor_focus_frame(CompositorSensei* c_sensei, u32 frame_id) {
    if (frame_id > c_sensei->frame_count || frame_id == 0)
        return;

    CompGridNode* node = &c_sensei->nodes[frame_id];
    FocusedNode* f_node = &c_sensei->focused_node;

    f_node->frame_id    = frame_id;
    f_node->row         = node->row;
    f_node->col         = node->col;
    f_node->col_segment = node->id;
}

static void comp_clean_borders(CompositorSensei* c_sensei) {
    VideoDriver* driver = &get_video_sensei()->driver;
    StyleColor colors     = dojo_get_theme()->palette.main_colors;

    for (u32 f = 1; f <= c_sensei->frame_count; f++) {
        CompWinBorder* border = &c_sensei->win_border[f];
        CompWinFrame*  frame  = &c_sensei->win_frame[f];

        for (u32 t = 0; t < border->width; t++) {
            // top border
            u32 on_corners = (t == 0) | (t + 1 == border->width);

            driver->draw_cell(c_sensei->window->framebuffer, 
                    border->start_height,
                    border->start_width + t,
                    ' ',
                    colors);
            // bottom border
            driver->draw_cell(c_sensei->window->framebuffer,
                    border->start_height + border->height -1,
                    border->start_width + t,
                    ' ',
                    colors);
        }
        for (u32 x = 1; x < border->height; x++) {
            // left border
            driver->draw_cell(c_sensei->window->framebuffer,
                    border->start_height + x,
                    border->start_width,
                    ' ',
                    colors);
            driver->draw_cell(c_sensei->window->framebuffer,
                    border->start_height + x,
                    border->start_width + border->width - 1,
                    ' ',
                    colors);
        }
    }
}

static void comp_draw_borders(CompositorSensei* c_sensei) {
    VideoDriver* driver = &get_video_sensei()->driver;
    StyleColor colors     = dojo_get_theme()->palette.main_colors;

    for (u32 f = 1; f <= c_sensei->frame_count; f++) {
        CompWinBorder* border = &c_sensei->win_border[f];
        CompWinFrame*  frame  = &c_sensei->win_frame[f];

        for (u32 t = 0; t < border->width; t++) {
            u32 on_left_cor = t == 0;
            u32 on_right_cor =  t + 1 == border->width;

            char top_char, bot_char;
            if (on_left_cor) {
                top_char = border->top_l_corner_c;
                bot_char = border->bot_l_corner_c;
            } else if (on_right_cor) {
                top_char = border->top_r_corner_c;
                bot_char = border->bot_r_corner_c;
            } else {
                top_char = border->hor_border_c;
                bot_char = border->bot_border_c;
            }

            // top border
            driver->draw_cell(c_sensei->window->framebuffer, 
                                border->start_height,
                                border->start_width + t,
                                top_char,
                                colors);
            // bottom border
            driver->draw_cell(c_sensei->window->framebuffer,
                                border->start_height + border->height -1,
                                border->start_width + t,
                                bot_char,
                                colors);
        }
        for (u32 x = 1; x < border->height - 1; x++) {
            // left border
            driver->draw_cell(c_sensei->window->framebuffer,
                                border->start_height + x,
                                border->start_width,
                                border->ver_border_c,
                                colors);
            driver->draw_cell(c_sensei->window->framebuffer,
                                border->start_height + x,
                                border->start_width + border->width - 1,
                                border->ver_border_c,
                                colors);
        }
    }
}

void comp_update_grid(CompositorSensei* c_sensei) {
    u32 base_height = c_sensei->window->height / c_sensei->grid.curr_max_rows;

    for (u32 f = 1; f <= c_sensei->frame_count; f++) {
        CompGridNode* node = &c_sensei->nodes[f];
        if (c_sensei->grid.curr_cols_in_row[node->row] ==0) continue;

        u32 base_width  = c_sensei->window->width / c_sensei->grid.curr_cols_in_row[node->row];

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
