#include "time.h"

#include "math.h"
#include <assert.h>
#include <math.h>
#include <time.h>

static void ticker_tick(struct ticker* ticker, struct duration delta);
static void ticker_reset(struct ticker* ticker);

void tick_timer(struct timer* const timer, const struct duration delta)
{
    ticker_tick(&timer->ticker, delta);
}

void reset_timer(struct timer* const timer)
{
    ticker_reset(&timer->ticker);
}

bool timer_expires(const struct timer* const timer)
{
    return get_elapsed_time_ratio(timer) >= 1;
}

float get_elapsed_time_ratio(const struct timer* const timer)
{
    assert(timer->duration.msecs != 0);
    return CLAMP(0, 1, (float)((double)timer->ticker.elapsed.msecs / (double)timer->duration.msecs));
}

struct duration get_remaining_time(const struct timer* const timer)
{
    return duration_diff(timer->duration, timer->ticker.elapsed);
}

struct duration get_overflow_time(const struct timer* const timer)
{
    return duration_diff(timer->ticker.elapsed, timer->duration);
}

void sleep_for(const struct duration duration)
{
    if (duration.msecs < 0) {
        return;
    }

    struct timespec time_spec = { 0 };
    time_spec.tv_sec = (time_t)duration.msecs / 1000;
    time_spec.tv_nsec = (time_t)duration.msecs % 1000 * 1000000;

    int slept = -1;
    do {
        slept = nanosleep(&time_spec, &time_spec);
    } while (slept != 0);
}

struct moment moment_from_duration(const struct duration duration, const enum time_precision precision)
{
    struct moment moment = { 0 };
    struct duration current = duration;

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

struct duration duration_from_moment(const struct moment moment)
{
    return (struct duration) {
        .msecs = moment.hours * time_hour + moment.mins * time_min + moment.secs * time_sec + moment.msecs * time_msec,
    };
}

struct duration duration_diff(const struct duration duration, const struct duration other)
{
    return (struct duration) {
        .msecs = duration.msecs > other.msecs ? duration.msecs - other.msecs : 0,
    };
}

struct duration get_monotonic_time(void)
{
    struct timespec time = { 0 };
    clock_gettime(CLOCK_MONOTONIC, &time);

    unsigned long msecs = 0;
    msecs += time.tv_sec * time_sec;
    msecs += (long)round((double)time.tv_nsec / 1.0e6);

    return (struct duration) { .msecs = msecs };
}

static void ticker_tick(struct ticker* const ticker, const struct duration delta)
{
    ticker->elapsed.msecs += delta.msecs;
}

static void ticker_reset(struct ticker* const ticker)
{
    ticker->elapsed.msecs = 0;
}
