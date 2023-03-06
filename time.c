#include "ccodoc.h"
#include <time.h>

extern int nanosleep(const struct timespec* requested_time, struct timespec* remaining);

void ticker_tick(ticker* clock, int delta_msec)
{
    clock->msec += delta_msec;
    if (clock->msec < 1000) {
        return;
    }

    int carried_sec = clock->msec / 1000;
    clock->sec += carried_sec;
    clock->msec = clock->msec - carried_sec * 1000;
}

void ticker_reset(ticker* clock)
{
    clock->sec = 0;
    clock->msec = 0;
}

int ticker_elapsed_msec(const ticker* clock)
{
    return clock->sec * 1000 + clock->msec;
}

void sleep_msec(int msec)
{
    if (msec < 0) {
        return;
    }

    struct timespec time_spec;
    time_spec.tv_sec = msec / 1000;
    time_spec.tv_nsec = (msec % 1000) * 1000000;

    int slept = -1;
    do {
        slept = nanosleep(&time_spec, &time_spec);
    } while (slept != 0);
}
