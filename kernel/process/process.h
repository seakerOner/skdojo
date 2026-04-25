#ifndef PROCESS_H
#define PROCESS_H

#include "../keyboard/keyboard_sensei.h"

typedef struct {
    u64 rax;
    u64 rbx;
    u64 rcx;
    u64 rdx;
    u64 rsi;
    u64 rdi;
    u64 rbp;
    u64 rsp;
    u64 r8, r9, r10, r11, r12, r13, r14, r15;
} CPUContext;

typedef enum {
    PROCESS_RUNNING,
    PROCESS_SLEEPING,
    PROCESS_DEAD
} ProcessState;

typedef struct {
    u64          pid;
    ProcessState state;
    void*        app_data;

    // void (*on_start)(void *);
    void ( *on_event   )( void*, KeyEvent* );
    void ( *on_update  )( void* );
    void ( *on_resize  )( void* app, u32 new_w, u32 new_h );
    void ( *on_destroy )( void* );

    // TODO:
    //
    // void (*entry)(void);
    // void *stack;
    // CPUContext cpu_ctx;

    //void (*on_render)(void*);
    //void (*on_destroy)(void*);
} DojoProcess;

#endif
