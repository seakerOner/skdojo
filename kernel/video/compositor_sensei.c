#include "./compositor_sensei.h"
#include "video_sensei.h"
#include "wmanager_sensei.h"

static CompositorSensei compositor_sensei = {0};

CompositorSensei* create_compositor_sensei( DojoWindow* window ) {

    //CompositorSensei compositor_sensei = {0};
    compositor_sensei.window = window;
    compositor_sensei.max_frames                = MAX_WINDOWS;
    compositor_sensei.focused_window            = &wmanager_get_focused()->id;
    compositor_sensei.grid.curr_max_rows        = 0;
    compositor_sensei.focused_node.row          = 0;
    compositor_sensei.focused_node.col          = 0;
    compositor_sensei.focused_node.col_segment  = 0;

    for ( u32 x = 0; x < MAX_WINDOWS; x++ ) {
        CompWinBorder* border = &compositor_sensei.win_border[x];

        border->hor_border_c    = 0xC4;
        border->ver_border_c    = 0xB3; 
        border->bot_border_c    = 0xC4;
        border->top_l_corner_c  = 0xDA;
        border->top_r_corner_c  = 0xBF;
        border->bot_l_corner_c  = 0xC0;
        border->bot_r_corner_c  = 0xD9;

        compositor_sensei.grid.curr_cols_in_row[x] = 0;
        compositor_sensei.alive_nodes[x] = FALSE;
    }

    for ( u32 r = 0; r < CS_GRID_ROWS; r++ ) {
        for ( u32 c = 0; c < CS_GRID_COLS; c++ ) {
            compositor_sensei.grid.curr_layers[r][c] = 0;

            for ( u32 l = 0; l < CS_GRID_LAYERS; l++ ) {
                compositor_sensei.grid.data[r][c][l] = 0;
            }
        }
    }

    return &compositor_sensei;
}

static void comp_clean_borders( CompositorSensei* c_sensei ); 
static void comp_draw_borders( CompositorSensei* c_sensei );

static inline u32 _get_valid_frame( CompositorSensei* c_sensei ) {
    for ( u32 x = 1; x <= c_sensei->max_frames - 1; x++ ) {
        if ( c_sensei->alive_nodes[x] == FALSE )
            return x;
    }
    return 0;   // no dead frames to resurrect 0_o
}

static inline void _shift_curr_cols_right( CompositorSensei* c_sensei, u32 insert_col ) {
    u32 row  = c_sensei->focused_node.row;
    u32 cols = c_sensei->grid.curr_cols_in_row[row];

    for ( u32 c = cols; c > insert_col; c-- ) {
        for ( u32 l = 0; l < c_sensei->grid.curr_layers[row][c-1]; l++ ) {
            u32 frame_id = c_sensei->grid.data[row][c-1][l];

            // shift to the node on the right
            c_sensei->grid.data[row][c][l] = frame_id;

            c_sensei->nodes[frame_id].col = c;
        }

        c_sensei->grid.curr_layers[row][c] = 
            c_sensei->grid.curr_layers[row][c-1];
    }

    c_sensei->grid.curr_layers[row][insert_col] = 0;
    comp_update_grid( c_sensei );
}

static inline void _shift_curr_cols_left( CompositorSensei* c_sensei, u32 insert_col ) {
    u32 row  = c_sensei->focused_node.row;
    u32 cols = c_sensei->grid.curr_cols_in_row[row];
    
    for ( u32 c = insert_col; c < cols - 1; c++ ) {
        for ( u32 l = 0; l < c_sensei->grid.curr_layers[row][c+1]; l++ ) {
            u32 frame_id  = c_sensei->grid.data[row][c+1][l];

            c_sensei->grid.data[row][c][l] = frame_id;

            c_sensei->nodes[frame_id].col = c;
        }
        
        c_sensei->grid.curr_layers[row][c] = 
            c_sensei->grid.curr_layers[row][c+1];
    }
    u32 last_col = cols - 1;

    for ( u32 l = 0; l < CS_GRID_LAYERS; l++ ) 
        c_sensei->grid.data[row][last_col][l] = 0;

    c_sensei->grid.curr_layers[row][last_col] = 0;

    comp_update_grid( c_sensei );
}

static inline void _shift_rows_after_focds_row( CompositorSensei* c_sensei, u32 insert_row ) {

    for ( u32 r = c_sensei->grid.curr_max_rows; r > insert_row; r-- ) {
        for ( u32 c = 0; c < c_sensei->grid.curr_cols_in_row[r-1]; c++ ) {
            for ( u32 l = 0; l < c_sensei->grid.curr_layers[r-1][c]; l++ ) {
                u32 frame_id = c_sensei->grid.data[r-1][c][l];

                c_sensei->grid.data[r][c][l] = frame_id;
                c_sensei->nodes[frame_id].row = r;
            }

            c_sensei->grid.curr_layers[r][c] = c_sensei->grid.curr_layers[r-1][c];
        }

        c_sensei->grid.curr_cols_in_row[r] = c_sensei->grid.curr_cols_in_row[r-1];
    }

    comp_update_grid( c_sensei );
}

static inline void _shift_compact_rows_after_focds_row( CompositorSensei* c_sensei, u32 insert_row ) {
    for ( u32 r = insert_row; r < c_sensei->grid.curr_max_rows - 1; r++ ) {
        for ( u32 c = 0; c < c_sensei->grid.curr_cols_in_row[r+1]; c++ ) {
            for ( u32 l = 0; l < c_sensei->grid.curr_layers[r+1][c]; l++ ) {
                u32 frame_id = c_sensei->grid.data[r+1][c][l];

                c_sensei->grid.data[r][c][l]  = frame_id;
                c_sensei->nodes[frame_id].row = r;
            }
            c_sensei->grid.curr_layers[r][c] = c_sensei->grid.curr_layers[r+1][c];
        }
        c_sensei->grid.curr_cols_in_row[r] = c_sensei->grid.curr_cols_in_row[r+1];
    }
    u32 last_row =  c_sensei->grid.curr_max_rows - 1;

    for ( u32 c = 0; c < c_sensei->grid.curr_cols_in_row[last_row]; c++ ) {
        for ( u32 l = 0; l < CS_GRID_LAYERS; l++ )
            c_sensei->grid.data[last_row][c][l] = 0;

        c_sensei->grid.curr_layers[last_row][c] = 0;
    }
    c_sensei->grid.curr_cols_in_row[last_row] = 0;

    comp_update_grid( c_sensei );
}

// returns NULL if frame is not created
CompWinFrame* compositor_create_frame_current_row( CompositorSensei* c_sensei ) {
    if ( c_sensei->grid.curr_cols_in_row[c_sensei->focused_node.row] >= CS_GRID_COLS )
        return NULL;
    if ( c_sensei->frame_count >= c_sensei->max_frames-1 )
        return NULL;

    if ( !c_sensei->grid.curr_max_rows )
        c_sensei->grid.curr_max_rows = 1;

    u32 row = c_sensei->focused_node.row;
    u32 cols = c_sensei->grid.curr_cols_in_row[row];
    u32 col = ( cols == 0 ) ? 0 : c_sensei->focused_node.col+1;

    if ( col < cols )
        _shift_curr_cols_right( c_sensei, col );

    c_sensei->grid.curr_cols_in_row[row]++;
    u32 layer = c_sensei->grid.curr_layers[row][col]++;

    u32 frame_id = _get_valid_frame( c_sensei );
    if ( frame_id == 0 )
        return NULL;

    c_sensei->frame_count++;

    CompWinFrame* frame   = &c_sensei->win_frame[frame_id];
    frame->id             = frame_id;
    frame->parent_window  = c_sensei->window;
    frame->process        = NULL;
    
    u32 grid_row = row;
    u32 grid_col = col;
    u32 grid_seg = layer;
    c_sensei->focused_node.col_segment = grid_seg;
   
    CompGridNode* node = &c_sensei->nodes[frame_id];

    node->id                        = grid_seg;
    node->row                       = row;
    node->col                       = col;
    node->resize_offsets.down       = 0;
    node->resize_offsets.up         = 0;
    node->resize_offsets.left       = 0;
    node->resize_offsets.right      = 0;
    c_sensei->alive_nodes[frame_id] = TRUE;
    c_sensei->grid.data[grid_row][grid_col][grid_seg] = frame_id;

    compositor_focus_frame( c_sensei, frame_id );

    comp_clean_borders( c_sensei );
    comp_update_grid(c_sensei);
    comp_clear( frame, dojo_get_theme()->palette.main_colors );

    // draw all border after calculations
    comp_draw_borders( c_sensei );

    return frame;
};

CompWinFrame* compositor_create_frame_new_row( CompositorSensei* c_sensei ) {
    if ( c_sensei->grid.curr_max_rows >= CS_GRID_ROWS )
        return NULL;
    if ( c_sensei->frame_count >= c_sensei->max_frames-1 )
        return NULL;

    comp_clean_borders( c_sensei );

    u32 new_row = c_sensei->focused_node.row+1;
    u32 col = 0;

    if ( new_row < c_sensei->grid.curr_max_rows )
        _shift_rows_after_focds_row( c_sensei, new_row );

    c_sensei->grid.curr_max_rows++;

    c_sensei->grid.curr_cols_in_row[new_row] = 0;  // ensure 0 init
    c_sensei->grid.curr_cols_in_row[new_row]++;

    u32 frame_id = _get_valid_frame( c_sensei );
    if ( frame_id == 0 )
        return NULL;

    c_sensei->frame_count++;

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

    node->resize_offsets.up         = 0;
    node->resize_offsets.down       = 0;
    node->resize_offsets.left       = 0;
    node->resize_offsets.right      = 0;
    c_sensei->alive_nodes[frame_id] = TRUE;

    c_sensei->grid.data[new_row][col][grid_seg] = frame_id;

    comp_update_grid( c_sensei );
    comp_draw_borders( c_sensei );

    return frame;
};

void compositor_focus_frame( CompositorSensei* c_sensei, u32 frame_id ) {
    if ( frame_id > c_sensei->max_frames || frame_id == 0 || !c_sensei->alive_nodes[frame_id] )
        return;

    CompGridNode* node = &c_sensei->nodes[frame_id];
    FocusedNode* f_node = &c_sensei->focused_node;

    f_node->frame_id    = frame_id;
    f_node->row         = node->row;
    f_node->col         = node->col;
    f_node->col_segment = node->id;

    comp_update_grid( c_sensei );
    comp_draw_borders( c_sensei );
}

static void comp_clean_borders( CompositorSensei* c_sensei ) {
    VideoDriver* driver = &get_video_sensei()->driver;
    StyleColor colors   = dojo_get_theme()->palette.main_colors;

    for ( u32 f = 1; f <= c_sensei->max_frames - 1; f++ ) {
        if (!c_sensei->alive_nodes[f]) continue;

        CompWinBorder* border = &c_sensei->win_border[f];

        for ( u32 t = 0; t < border->width; t++ ) {
            // top border
            driver->draw_cell( c_sensei->window->framebuffer, 
                    border->start_height,
                    border->start_width + t,
                    ' ',
                    colors );
            // bottom border
            driver->draw_cell( c_sensei->window->framebuffer,
                    border->start_height + border->height -1,
                    border->start_width + t,
                    ' ',
                    colors );
        }
        for ( u32 x = 1; x < border->height; x++ ) {
            // left border
            driver->draw_cell( c_sensei->window->framebuffer,
                    border->start_height + x,
                    border->start_width,
                    ' ',
                    colors );
            driver->draw_cell( c_sensei->window->framebuffer,
                    border->start_height + x,
                    border->start_width + border->width - 1,
                    ' ',
                    colors );
        }
    }
}

static void comp_draw_borders( CompositorSensei* c_sensei ) {
    VideoDriver* driver = &get_video_sensei()->driver;
    StyleColor colors;     

    for ( u32 f = 1; f <= c_sensei->max_frames - 1; f++ ) {
        if ( !c_sensei->alive_nodes[f] ) continue;

        CompWinBorder* border = &c_sensei->win_border[f];
        CompWinFrame*  frame  = &c_sensei->win_frame[f];
        if ( frame->id == c_sensei->focused_node.frame_id )
            colors = dojo_get_theme()->palette.focus_color;
        else
            colors = dojo_get_theme()->palette.main_colors;

        for ( u32 t = 0; t < border->width; t++ ) {
            u32 on_left_cor = t == 0;
            u32 on_right_cor =  t + 1 == border->width;

            ascii top_char, bot_char;
            if ( on_left_cor ) {
                top_char = border->top_l_corner_c;
                bot_char = border->bot_l_corner_c;
            } else if ( on_right_cor ) {
                top_char = border->top_r_corner_c;
                bot_char = border->bot_r_corner_c;
            } else {
                top_char = border->hor_border_c;
                bot_char = border->bot_border_c;
            }

            // top border
            driver->draw_cell( c_sensei->window->framebuffer, 
                                border->start_height,
                                border->start_width + t,
                                top_char,
                                colors );
            // bottom border
            driver->draw_cell( c_sensei->window->framebuffer,
                                border->start_height + border->height -1,
                                border->start_width + t,
                                bot_char,
                                colors );
        }
        for ( u32 x = 1; x < border->height - 1; x++ ) {
            // left border
            driver->draw_cell( c_sensei->window->framebuffer,
                                border->start_height + x,
                                border->start_width,
                                border->ver_border_c,
                                colors );
            driver->draw_cell( c_sensei->window->framebuffer,
                                border->start_height + x,
                                border->start_width + border->width - 1,
                                border->ver_border_c,
                                colors );
        }
    }
}

void comp_update_grid( CompositorSensei* c_sensei ) {
    if ( c_sensei->grid.curr_max_rows == 0 )
        return;
    if ( c_sensei->window->height == 0 || c_sensei->window->width == 0 )
         return;

    u32 base_height = c_sensei->window->height / c_sensei->grid.curr_max_rows;
    u32 h_remainder = c_sensei->window->height % c_sensei->grid.curr_max_rows;

    for ( u32 f = 1; f <= c_sensei->max_frames - 1; f++ ) {
        if ( !c_sensei->alive_nodes[f] ) continue;

        CompGridNode* node = &c_sensei->nodes[f];
        if ( c_sensei->grid.curr_cols_in_row[node->row] == 0 ) continue;

        u32 base_width  = c_sensei->window->width / c_sensei->grid.curr_cols_in_row[node->row];
        u32 remainder   = c_sensei->window->width % c_sensei->grid.curr_cols_in_row[node->row];

        //
        // TODO: use node resize offsets to make frames resizable

        u32 abs_id = ( u32 )c_sensei->grid.data[node->row][node->col][node->id];
        if ( !abs_id ) continue;

        u32 row_todraw     = base_height * node->row;
        u32 abs_height     = base_height;

        u32 col_todraw     = base_width * node->col;
        u32 abs_width      = base_width;

        if ( node->col == c_sensei->grid.curr_cols_in_row[node->row] - 1 ) {
            abs_width += remainder;
            col_todraw = c_sensei->window->width - abs_width;
        }
        if ( node->row == c_sensei->grid.curr_max_rows - 1 ) {
            abs_height += h_remainder;
            row_todraw  = c_sensei->window->height - abs_height; 
        }

        CompWinBorder* border = &c_sensei->win_border[abs_id];
        border->start_width   = col_todraw;
        border->start_height  = row_todraw;
        border->height        = abs_height;
        border->width         = abs_width;

        CompWinFrame* frame   = &c_sensei->win_frame[abs_id];
        frame->start_width    = col_todraw + 1;
        frame->start_height   = row_todraw + 1;
        frame->width          = abs_width  - 2;
        frame->height         = abs_height - 2;

        comp_clear( frame, dojo_get_theme()->palette.main_colors ); // ensure no artifacts
        if ( frame->process && frame->process->on_resize )
            frame->process->on_resize( frame->process->app_data, frame->width, frame->height );
    }

}

void compositor_focus_up( CompositorSensei* c_sensei ) {
    FocusedNode* fn = &c_sensei->focused_node;

    if ( fn->row == 0 )
        return;

    u32 target_col = ( c_sensei->grid.curr_cols_in_row[fn->row-1] <= fn->col ) ? 
                    c_sensei->grid.curr_cols_in_row[fn->row-1] - 1 : fn->col ;

    // layer 0 is assumed to be the main app layer 
    u32 frame_id = c_sensei->grid.data[fn->row-1][target_col][0];

    compositor_focus_frame( c_sensei, frame_id );
}

void compositor_focus_down( CompositorSensei* c_sensei ) {
    FocusedNode* fn = &c_sensei->focused_node;

    if ( fn->row+1 >= c_sensei->grid.curr_max_rows )
        return;
    if ( c_sensei->grid.curr_cols_in_row[fn->row+1] == 0 )
        return;

    u32 target_col = ( c_sensei->grid.curr_cols_in_row[fn->row+1] <= fn->col ) ? 
                    c_sensei->grid.curr_cols_in_row[fn->row+1] - 1 : fn->col ;

    // layer 0 is assumed to be the main app layer
    u32 frame_id = c_sensei->grid.data[fn->row+1][target_col][0];
    compositor_focus_frame( c_sensei, frame_id );
}
void compositor_focus_left(CompositorSensei* c_sensei) {
    FocusedNode* fn = &c_sensei->focused_node;

    if ( fn->col == 0 )
        return;

    u32 target_col = fn->col - 1;

    // layer 0 is assumed to be the main app layer
    u32 frame_id = c_sensei->grid.data[fn->row][target_col][0];
    compositor_focus_frame( c_sensei, frame_id );
}
void compositor_focus_right( CompositorSensei* c_sensei ) {
    FocusedNode* fn = &c_sensei->focused_node;

    if ( fn->col+1 >= c_sensei->grid.curr_cols_in_row[fn->row] )
        return;

    u32 target_col = fn->col + 1;

    // layer 0 is assumed to be the main app layer
    u32 frame_id = c_sensei->grid.data[fn->row][target_col][0];
    compositor_focus_frame( c_sensei, frame_id );
}

CompWinFrame* compositor_get_focused_frame( CompositorSensei* c_sensei ) {
    FocusedNode* fn = &c_sensei->focused_node;
    return &c_sensei->win_frame[fn->frame_id];
}
void compositor_destroy_focused_frame( CompositorSensei *c_sensei ) {
    if ( c_sensei->frame_count <= 1 )     // there should always exist atleast 1 frame
        return;

    FocusedNode  fn      = c_sensei->focused_node;
    CompWinFrame* fframe = &c_sensei->win_frame[fn.frame_id];
    u32 frame_id         = fn.frame_id;

    comp_clean_borders( c_sensei );
    comp_clear( fframe, dojo_get_theme()->palette.main_colors );

    // let process terminate inside frame 
    if ( fframe->process && fframe->process->on_destroy )
        fframe->process->on_destroy( fframe->process->app_data );

    if ( c_sensei->grid.curr_cols_in_row[fn.row] > 1 ) {  // remove a col in the current row
        _shift_curr_cols_left( c_sensei, fn.col );
        c_sensei->grid.curr_cols_in_row[fn.row]--;
    } else {                                            // remove current row
        _shift_compact_rows_after_focds_row( c_sensei, fn.row );
        c_sensei->grid.curr_max_rows--;
    }

    c_sensei->alive_nodes[frame_id] = FALSE;

    c_sensei->frame_count--;

    FILL( ( u8* )&c_sensei->nodes[frame_id], 0, sizeof( CompGridNode ) );
    FILL(( u8* )&c_sensei->win_frame[frame_id], 0, sizeof( CompWinFrame ) );

    u32 new_focus = 1;
    for ( u32 x = MAX_WINDOWS - 1; x > 0; x-- ) 
        if ( c_sensei->alive_nodes[x] ) {
            new_focus = x;
            break;
        }

    compositor_focus_frame( c_sensei, new_focus );
    comp_update_grid( c_sensei );
    comp_draw_borders( c_sensei );
}

void comp_draw_cell( CompWinFrame* frame, u32 row, u32 col, ascii c, StyleColor style ) {
    get_video_sensei()->driver.draw_cell( frame->parent_window->framebuffer, 
                                                row + frame->start_height,
                                                col + frame->start_width,
                                                c, style );
};

void comp_read_cell( CompWinFrame* frame, u32 row, u32 col, ascii* out, StyleColor* style_out ) {
    get_video_sensei()->driver.read_cell( frame->parent_window->framebuffer,
                                                row + frame->start_height,
                                                col + frame->start_width,
                                                out, style_out );
};
void comp_clear( CompWinFrame* frame, const StyleColor colors ) {
    for ( u32 r = 0; r < frame->height; r++ ) {
        for ( u32 c = 0; c < frame->width; c++ ) {
            get_video_sensei()->driver.draw_cell( frame->parent_window->framebuffer,
                                                r + frame->start_height,
                                                c + frame->start_width,
                                                ' ',
                                                colors );
        }
    }
};
