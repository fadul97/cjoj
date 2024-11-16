#include "platform/timer.h"

#if JPLATFORM_WINDOWS

#include <windows.h>
#include <mmsystem.h> // timeBeginPeriod and timeEndPeriod functions

typedef struct Win32Timer_st
{
    LARGE_INTEGER counter_start;
    LARGE_INTEGER end;
    LARGE_INTEGER freq;
    b8 stopped;
    f64 cumulative_elapsed;
} JTimer;

static b8 initialized = FALSE;
static JTimer timer = { 0 };

void time_create()
{
    // Get frequency from high-resolution counter
    if (!timer.freq.QuadPart) {
        QueryPerformanceFrequency(&timer.freq);
    }

    // Reset the start and end values of the counter
    ZeroMemory(&timer.counter_start, sizeof(timer.counter_start));
    ZeroMemory(&timer.end, sizeof(timer.end));

    // Timer running
    timer.stopped = FALSE;
    timer.cumulative_elapsed = 0.0f;

    initialized = TRUE;
}

void time_start()
{
    if (timer.stopped)
    {
        // Resume count time
        //
        //      <--- elapsed ---->
        // ----|------------------|------------> time
        //    start               end
        //

        // Time elapsed before stopping
        const i64 elapsed = timer.end.QuadPart - timer.counter_start.QuadPart;

        // Takes into account time already elapsed before the stop
        QueryPerformanceCounter(&timer.counter_start);
        timer.counter_start.QuadPart -= elapsed;

        // Resume normal counting
        timer.stopped = FALSE;
    }
    else
    {
        // Start counting time
        QueryPerformanceCounter(&timer.counter_start);
    }
}

void time_stop()
{
    if (!timer.stopped)
    {
        // Mark the stopping point of time
        QueryPerformanceCounter(&timer.end);
        timer.stopped = TRUE;
    }
}

f64 time_reset()
{
    // To keep track of time elapsed
    i64 elapsed;

    if (timer.stopped)
    {
        // Get time elapsed before stopping
        elapsed = timer.end.QuadPart - timer.counter_start.QuadPart;

        // Reset time count
        QueryPerformanceCounter(&timer.counter_start);

        // Count reactivated
        timer.stopped = FALSE;
    }
    else
    {
        // End time counting
        QueryPerformanceCounter(&timer.end);

        // Calculate elapsed time (in cycles)
        elapsed = timer.end.QuadPart - timer.counter_start.QuadPart;

        // Reset counter
        timer.counter_start = timer.end;
    }

    // Add to cumulative elapsed time
    timer.cumulative_elapsed += elapsed / (f64)(timer.freq.QuadPart);

    // Convert time to seconds
    return elapsed / (f64)(timer.freq.QuadPart);
}

f64 time_elapsed()
{
    // To keep track of time elapsed
    i64 elapsed;

    if (timer.stopped)
    {
        // Get time elapsed until the stop
        elapsed = timer.end.QuadPart - timer.counter_start.QuadPart;

    }
    else
    {
        // End time counting
        QueryPerformanceCounter(&timer.end);

        // Calculate elapsed time (in cycles)
        elapsed = timer.end.QuadPart - timer.counter_start.QuadPart;
    }

    // Convert time to seconds
    return elapsed / (f64)(timer.freq.QuadPart);
}

b8 time_was_elapsed(f64 secs)
{
    return (time_elapsed() >= secs ? TRUE : FALSE);
}

b8 time_is_cumulative_elapsed_more_than(f64 secs)
{
    return (timer.cumulative_elapsed >= secs ? TRUE : FALSE);
}

f64 time_total_elapsed()
{
    return timer.cumulative_elapsed;
}

long long time_stamp()
{
    QueryPerformanceCounter(&timer.end);
    return timer.end.QuadPart;
}

f64 time_elapsed_since(long long stamp)
{
    // To keep track of time elapsed
    i64 elapsed;

    if (timer.stopped)
    {
        // Get time elapsed until the stop
        elapsed = timer.end.QuadPart - stamp;

    }
    else
    {
        // End time counting
        QueryPerformanceCounter(&timer.end);

        // Calculate elapsed time (in cycles)
        elapsed = timer.end.QuadPart - stamp;
    }

    // Convert time to seconds
    return elapsed / (f64)(timer.freq.QuadPart);
}

b8 time_was_elapsed_since(long long stamp, f64 secs)
{
    return (time_elapsed_since(stamp) >= secs ? TRUE : FALSE);
}

void time_begin_period()
{
    timeBeginPeriod(1);
}

void time_end_period()
{
    timeEndPeriod(1);
}

#endif // JPLATFORM_WINDOWS