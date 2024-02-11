#pragma once

#include <stdbool.h>
#include <time.h> // IWYU pragma: export

enum time_precision {
    time_msec = 1,
    time_sec = 1000 * time_msec,
    time_min = 60 * time_sec,
    time_hour = 60 * time_min,
};

struct moment {
    unsigned int hours;
    unsigned int mins;
    unsigned int secs;
    unsigned int msecs;
};

struct duration {
    unsigned long msecs;
};

struct ticker {
    struct duration elapsed;
};

struct timer {
    struct duration duration;
    struct ticker ticker;
};

// - timer
extern void tick_timer(struct timer* timer, const struct duration delta);
extern void reset_timer(struct timer* timer);
extern bool timer_expires(const struct timer* timer);
extern float get_elapsed_time_ratio(const struct timer* timer);
extern struct duration get_remaining_time(const struct timer* timer);
extern struct duration get_overflow_time(const struct timer* timer);

// - sleep
extern void sleep_for(const struct duration duration);

// - moment
extern struct moment moment_from_duration(const struct duration duration, enum time_precision precision);

// - duration
extern struct duration duration_from_moment(const struct moment moment);
extern struct duration duration_diff(const struct duration duration, const struct duration other);
extern struct duration get_monotonic_time(void);
