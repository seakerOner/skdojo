#include "time_sensei.h"

static TimeSensei time_sensei = {0};

void start_time_sensei() {
    time_sensei.ticks = get_time_ticker();
};

u64 dojo_get_timestamp() {
    // TODO: use a high resolution clock for more precise timestamps (after APIC)
    return *time_sensei.ticks;
};

static inline void halt() {
    __asm__ __volatile__("hlt");
}

// this sleep greatly reduces power consumption but blocks the whole cpu core
// this should be used mainly be the scheduler when it wants to into IDLE state 
// 
// scheduler must create a process by process sleep call
void dojo_sleep_ms(u64 ms) {
    u64 target_ticks = dojo_get_timestamp() + MS_TO_TICKS(ms);

    // a sleep call will do at minimum 1 halt "cycle"
    do {
        halt();
    } while (dojo_get_timestamp() < target_ticks);
}

