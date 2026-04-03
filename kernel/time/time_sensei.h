#ifndef TIME_SENSEI_H
#define TIME_SENSEI_H

#include "../interrupts/k_interrupts.h"

typedef struct {
    volatile u64* ticks;
} TimeSensei;

void start_time_sensei();

u64 dojo_get_timestamp();

void dojo_sleep_ms(u64 ms);

#endif
