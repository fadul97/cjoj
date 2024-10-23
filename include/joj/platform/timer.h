#ifndef JOJ_TIMER_H
#define JOJ_TIMER_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

struct JTimer;

void time_create();
void time_start();
void time_stop();
f64 time_reset();
f64 time_elapsed();
b8 time_was_elapsed(f64 secs);
b8 time_is_cumulative_elapsed_more_than(f64 secs);
f64 time_total_elapsed();
long long time_stamp();
f64 time_elapsed_since(long long stamp);
b8 time_was_elapsed_since(long long stamp, f64 secs);

void time_begin_period();
void time_end_period();

#endif // JOJ_TIMER_WIN32_H