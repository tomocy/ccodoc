#include "time.h"

#include "math.h"
#include <assert.h>
#include <math.h>
#include <time.h>

static void ticker_tick(ticker_t* ticker, const duration_t delta);
static void ticker_reset(ticker_t* ticker);

void tick_timer(tick_timer_t* const timer, const duration_t delta)
{
    ticker_tick(&timer->ticker, delta);
}

void reset_timer(tick_timer_t* const timer)
{
    ticker_reset(&timer->ticker);
}

bool timer_expires(const tick_timer_t* const timer)
{
    return elapsed_time_ratio(timer) >= 1;
}

float elapsed_time_ratio(const tick_timer_t* const timer)
{
    assert(timer->duration.msecs != 0);
    return CLAMP(0, 1, (float)((double)timer->ticker.elapsed.msecs / (double)timer->duration.msecs));
}

duration_t remaining_time(const tick_timer_t* const timer)
{
    return duration_diff(timer->duration, timer->ticker.elapsed);
}

duration_t overflow_time(const tick_timer_t* const timer)
{
    return duration_diff(timer->ticker.elapsed, timer->duration);
}

void sleep_for(const duration_t duration)
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

moment_t moment_from_duration(const duration_t duration, const time_precision_t precision)
{
    moment_t moment = { 0 };
    duration_t current = duration;

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

duration_t duration_from_moment(const moment_t moment)
{
    return (duration_t) {
        .msecs = moment.hours * time_hour + moment.mins * time_min + moment.secs * time_sec + moment.msecs * time_msec,
    };
}

duration_t duration_diff(const duration_t duration, const duration_t other)
{
    return (duration_t) {
        .msecs = duration.msecs >= other.msecs ? duration.msecs - other.msecs : 0,
    };
}

duration_t monotonic_time(void)
{
    struct timespec time = { 0 };
    clock_gettime(CLOCK_MONOTONIC, &time);

    unsigned long msecs = 0;
    msecs += time.tv_sec * time_sec;
    msecs += (long)round((double)time.tv_nsec / 1.0e6);

    return (duration_t) { .msecs = msecs };
}

static void ticker_tick(ticker_t* const ticker, const duration_t delta)
{
    ticker->elapsed.msecs += delta.msecs;
}

static void ticker_reset(ticker_t* const ticker)
{
    ticker->elapsed.msecs = 0;
}
