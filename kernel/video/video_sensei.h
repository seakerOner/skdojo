#ifndef VIDEO_SENSEI_H
#define VIDEO_SENSEI_H

#include "../drivers/video/vga/vga_driver.h"
#include "../themes/themes.h"

// different types of video output
typedef enum {
    VGA_MODE,
} VideoType;

typedef struct {
    void (*putc)            (void* fb, const char c, const StyleColor colors);
    void (*print)           (void* fb, const char* msg, const StyleColor colors);
    void (*clear)           (void* fb, const StyleColor colors);
    void (*scroll)          (void* fb, const StyleColor colors);
    void (*newline)         (void* fb, const StyleColor colors);
    void (*gotoline)        (void* fb, const u32 line, const StyleColor colors);
    void* (*get_framebuffer) ();
} VideoDriver;

typedef struct {
    VideoDriver driver;
    u64 screen_width;
    u64 screen_height;
} VideoSensei;

void create_video_sensei(); 

VideoSensei* get_video_sensei();

#endif 
