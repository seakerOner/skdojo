#ifndef VIDEO_SENSEI_H
#define VIDEO_SENSEI_H

//
// Drivers must provide minimal functionality that the VideoSensei expects.
// See the `VideoDriver` struct
//
// A framebuffer size must not be greater than the macro below 
//
// NOTE:
// this macro is a hack to have static memory that can hold a framebuffer at compile time.
// Since the Sensei doesnt know what type of videobuffer it holds the prefered way would be 
// to dynamically allocate it.. but we don't have a memory model made yet
//
# define DRIVER_MAX_FRAMEBUFFER_LEN_BYTES 32

#include "../drivers/video/vga/vga_driver.h"
#include "../themes/themes.h"

// different types of video output
typedef enum {
    VGA_MODE,
} VideoType;

typedef struct {
    void ( *draw_cell )        ( void* fb, u32 row, u32 col, ascii c, StyleColor style );
    void ( *read_cell )        ( void* fb, u32 row, u32 col, ascii* out, StyleColor* style_out );
    void ( *clear )            ( void* fb, const StyleColor colors );

    u64  ( *framebuffer_size ) (); // use this with allocation when mem model is done
    void ( *new_framebuffer )  ( u32 start_row, u32 start_col, 
            u32 max_collumns, u32 max_rows, 
            void* out_fb );
} VideoDriver;

typedef struct {
    u8 data[DRIVER_MAX_FRAMEBUFFER_LEN_BYTES];
} FrameBuffer;

typedef FrameBuffer* FrameBufferHandle;

typedef struct {
    VideoDriver driver;
    u64 screen_width;
    u64 screen_height;
} VideoSensei;

VideoSensei* create_video_sensei(); 

VideoSensei* get_video_sensei();

#endif 
