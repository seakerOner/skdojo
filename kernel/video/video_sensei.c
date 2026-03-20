#include "video_sensei.h"

VideoSensei video_sensei;

void create_video_sensei() {
    // TODO: Check VGA support via PCI
    // hardcoded to VGA for now
    video_sensei.driver.putc     = (void *)driver_vga_putc;
    video_sensei.driver.print    = (void *)driver_vga_print;
    video_sensei.driver.clear    = (void *)driver_vga_clear;
    video_sensei.driver.scroll   = (void *)driver_vga_scroll;
    video_sensei.driver.gotoline = (void *)driver_vga_gotoline;
    video_sensei.driver.newline  = (void *)driver_vga_newline;
    video_sensei.driver.get_framebuffer= (void *)driver_vga_get_framebuffer;
 
    video_sensei.screen_width  = VGA_COLUMNS;
    video_sensei.screen_height = VGA_ROWS;
}

VideoSensei* get_video_sensei() {
    return &video_sensei;
}

const VideoDriver* sensei_video_get_drive() {
    return &video_sensei.driver;
}
