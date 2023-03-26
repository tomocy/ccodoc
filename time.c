#include "time.h"

#include "math.h"
#include <assert.h>
#include <math.h>
#include <time.h>

extern int nanosleep(const struct timespec* requested_time, struct timespec* remaining);

static void ticker_tick(ticker* ticker, const duration delta);
static void ticker_reset(ticker* ticker);

void tick_timer(timer* timer, const duration delta)
{
    if (is_timeout(timer)) {
        return;
    }

    ticker_tick(&timer->ticker, delta);
}

void reset_timer(timer* timer)
{
    ticker_reset(&timer->ticker);
}

bool is_timeout(const timer* timer)
{
    return elapsed_time_ratio(timer) >= 1;
}

float elapsed_time_ratio(const timer* timer)
{
    assert(timer->duration.msecs != 0);
    return CLAMP(0, 1, (float)((double)timer->ticker.elapsed.msecs / (double)timer->duration.msecs));
}

duration remaining_time(const timer* timer)
{
    return (duration) {
        .msecs = timer->duration.msecs - MIN(timer->ticker.elapsed.msecs, timer->duration.msecs),
    };
}

void sleep_for(const duration duration)
{
    if (duration.msecs < 0) {
        return;
    }

    struct timespec time_spec;
    time_spec.tv_sec = (time_t)duration.msecs / 1000;
    time_spec.tv_nsec = (time_t)duration.msecs % 1000 * 1000000;

    int slept = -1;
    do {
        slept = nanosleep(&time_spec, &time_spec);
    } while (slept != 0);
}

moment moment_from_duration(const duration d, time_precision precision)
{
    moment moment = { 0 };
    duration current = d;

    if (precision <= time_hour) {
        double hours = (double)current.msecs / time_hour;
        if (precision == time_hour) {
            hours = ceil(hours);
        }
        moment.hours = (unsigned int)hours;

        current.msecs -= (unsigned long)moment.hours * time_hour;
    }

    if (precision <= time_min) {
        double mins = (double)current.msecs / time_min;
        if (precision == time_min) {
            mins = ceil(mins);
        }
        mins = MIN(mins, 59);

        moment.mins = (unsigned int)mins;

        current.msecs -= (unsigned long)moment.mins * time_min;
    }

    if (precision <= time_sec) {
        double secs = (double)current.msecs / time_sec;
        if (precision == time_sec) {
            secs = ceil(secs);
        }
        secs = MIN(secs, 59);

        moment.secs = (unsigned int)secs;

        current.msecs -= (unsigned long)moment.secs * time_sec;
    }

    if (precision <= time_msec) {
        double msecs = (double)current.msecs / time_msec;
        if (precision == time_msec) {
            msecs = ceil(msecs);
        }
        msecs = MIN(msecs, 999);

        moment.msecs = (unsigned int)msecs;

        current.msecs -= (unsigned long)moment.msecs * time_msec;
    }

    return moment;
}

duration duration_from_moment(const moment moment)
{
    return (duration) {
        .msecs = moment.hours * time_hour + moment.mins * time_min + moment.secs * time_sec + moment.msecs * time_msec,
    };
}

duration duration_diff(const duration d, const duration other)
{
    return (duration) {
        .msecs = d.msecs >= other.msecs ? d.msecs - other.msecs : 0,
    };
}

duration monotonic_time(void)
{
    struct timespec time = { 0 };
    clock_gettime(CLOCK_MONOTONIC, &time);

    unsigned long msecs = 0;
    msecs += time.tv_sec * time_sec;
    msecs += (long)round((double)time.tv_nsec / 1.0e6);

    return (duration) { .msecs = msecs };
}

static void ticker_tick(ticker* ticker, const duration delta)
{
    ticker->elapsed.msecs += delta.msecs;
}

static void ticker_reset(ticker* ticker)
{
    ticker->elapsed.msecs = 0;
}
