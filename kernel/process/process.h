#ifndef PROCESS_H
#define PROCESS_H

#include "../keyboard/keyboard_sensei.h"
#include "../bios_boot_info.h"

#define MAX_PROCESS_EVENTS 162

typedef enum {
    DJ_EVENT_NONE = 0,

    DJ_EVENT_KEYBOARD,
    DJ_EVENT_WINDOW_RESIZE,
    DJ_EVENT_WINDOW_CLOSE,

    DJ_EVENT_PROCESS_MESSAGE,

    DJ_EVENT_TIMER,
} DojoEventType;

typedef struct {
    DojoEventType type;

    union {
        KeyEvent keyboard;

        struct {
            u32 width;
            u32 height;
        } resize;

        struct {
            u64 from_pid;
            void* data;
        } message;
    };
} DojoEvent;

typedef struct {
    u64 ip;
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
    PROC_RUNNING,
    PROC_SLEEPING,
    PROC_DEAD
} ProcessState;

typedef enum {
    NATIVE_PROC,
    FORTH_PROC,
} ProcessType;

typedef struct DojoProcess_t DojoProcess;

typedef void (*ProcessEntry)(DojoProcess* self);

typedef struct DojoProcess_t {
    u64          pid;

    ProcessState state;
    ProcessType  type;

    ProcessEntry entry;
    void *stack;

    CPUContext cpu_ctx;
    // Pml4Table* pml4;

    DojoEvent events[MAX_PROCESS_EVENTS];
    u64 event_read;
    u64 event_write;

} DojoProcess;

typedef struct {
    ProcessType type;
    ProcessEntry entry;
} DojoProcessSpawnConfig;

boolean dojo_process_push_event(DojoProcess* process, DojoEvent* event);
boolean dojo_process_pop_event(DojoProcess* process, DojoEvent* event);

void dojo_dispatch_event(DojoProcess* process, DojoEvent* event);

#endif
