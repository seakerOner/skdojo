#ifndef COMPOSITOR_SENSEI_H
#define COMPOSITOR_SENSEI_H

#include "wmanager_sensei.h"
#include "../keyboard/keyboard_sensei.h"
#include "../process/process.h"

#define CS_GRID_ROWS MAX_WINDOWS
#define CS_GRID_COLS MAX_WINDOWS
#define CS_GRID_LAYERS MAX_WINDOWS

typedef struct {
    u32 start_width;
    u32 start_height;
    u32 width;
    u32 height;

    char hor_border_c;
    char ver_border_c;
    char bot_border_c;

    char top_l_corner_c;     
    char top_r_corner_c;     
    char bot_r_corner_c;     
    char bot_l_corner_c;     
} CompWinBorder;

typedef struct {
    DojoWindow* parent_window;
    DojoProcess* process;
    u32  id;
    u32  start_width;
    u32  start_height;
    u32  width;
    u32  height;
} CompWinFrame;

typedef struct {
    u32 frame_id;
    u32 row;
    u32 col;
    u32 col_segment;
} FocusedNode;

// will be used so each frame can have N layers (popup windows, notifications, menus, etc..)
typedef struct {
    u32 curr_max_rows;
    u32 curr_cols_in_row[CS_GRID_ROWS];
    u32 curr_layers[CS_GRID_ROWS][CS_GRID_COLS];
    u32 data[CS_GRID_ROWS][CS_GRID_COLS][CS_GRID_LAYERS]; 
} CompGrid; 

typedef struct {
    u32 row;
    u32 col;
    u32 id;
    struct {      // when user wants to resize a window
        u32 up, down, left, right;
    } resize_offsets;
} CompGridNode;

typedef struct {
    DojoWindow*     window;
    u32*            focused_window;     // reference from Window Manager Sensei
    u32             frame_count;
    u32             max_frames;
    FocusedNode     focused_node;
    CompWinBorder   win_border[MAX_WINDOWS];
    CompWinFrame    win_frame[MAX_WINDOWS];
    CompGridNode    nodes[MAX_WINDOWS];
    CompGrid        grid;
} CompositorSensei;

CompositorSensei* create_compositor_sensei(DojoWindow* window);

CompWinFrame* compositor_create_window_current_row(CompositorSensei* c_sensei);
CompWinFrame* compositor_create_window_new_row(CompositorSensei* c_sensei);

void comp_update_grid(CompositorSensei* c_sensei);

void compositor_focus_frame(CompositorSensei* c_sensei, u32 frame_id);

void compositor_focus_up(CompositorSensei* c_sensei);
void compositor_focus_down(CompositorSensei* c_sensei);
void compositor_focus_left(CompositorSensei* c_sensei);
void compositor_focus_right(CompositorSensei* c_sensei);

CompWinFrame* compositor_get_focused_frame(CompositorSensei* c_sensei);

// 1 -> polled | 0 ->ignored
u32 compositor_poll(CompositorSensei* c_sensei, KeyEvent* ev);

void comp_draw_cell(CompWinFrame* frame, u32 row, u32 col, char c, StyleColor style);
void comp_read_cell(CompWinFrame* frame, u32 row, u32 col, char* out, StyleColor* style_out);
void comp_clear    (CompWinFrame* frame, const StyleColor colors);

#endif
