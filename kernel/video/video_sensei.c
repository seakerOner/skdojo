#include "video_sensei.h"

#include "../printk/printk.h"

static VideoSensei video_sensei;

VideoSensei* create_video_sensei() {
    // TODO: Check VGA support via PCI
    // hardcoded to VGA for now


    video_sensei.driver.draw_cell           = ( void * )driver_vga_draw_cell;
    video_sensei.driver.read_cell           = ( void * )driver_vga_read_cell;
    video_sensei.driver.clear               = ( void * )driver_vga_clear;
    video_sensei.driver.framebuffer_size    = ( void * )driver_vga_framebuffer_size;
    video_sensei.driver.new_framebuffer     = ( void * )driver_vga_new_framebuffer;

    video_sensei.screen_width  = VGA_COLUMNS;
    video_sensei.screen_height = VGA_ROWS;

    if ( video_sensei.driver.framebuffer_size() > DRIVER_MAX_FRAMEBUFFER_LEN_BYTES ) {
        //printk_warn("[VIDEO SENSEI WARNING] Framebuffer bigger than permited by Video Sensei.\n");
        // hang
        while (1);
    }

    return &video_sensei;
}

VideoSensei* get_video_sensei() {
    return &video_sensei;
}

const VideoDriver* sensei_video_get_drive() {
    return &video_sensei.driver;
}
