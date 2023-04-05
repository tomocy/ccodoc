#pragma once

#include <stdbool.h>
#include <time.h>

typedef enum {
    time_msec = 1,
    time_sec = 1000 * time_msec,
    time_min = 60 * time_sec,
    time_hour = 60 * time_min,
} time_precision_t;

typedef struct {
    unsigned int hours;
    unsigned int mins;
    unsigned int secs;
    unsigned int msecs;
} moment_t;

typedef struct {
    unsigned long msecs;
} duration_t;

typedef struct {
    duration_t elapsed;
} ticker_t;

typedef struct {
    duration_t duration;
    ticker_t ticker;
} tick_timer_t;

// - timer
extern void tick_timer(tick_timer_t* timer, const duration_t delta);
extern void reset_timer(tick_timer_t* timer);
extern bool timer_expires(const tick_timer_t* timer);
extern float elapsed_time_ratio(const tick_timer_t* timer);
extern duration_t remaining_time(const tick_timer_t* timer);
extern duration_t overflow_time(const tick_timer_t* timer);

// - sleep
extern void sleep_for(const duration_t duration);

// - moment
extern moment_t moment_from_duration(const duration_t duration, time_precision_t precision);

// - duration
extern duration_t duration_from_moment(const moment_t moment);
extern duration_t duration_diff(const duration_t duration, const duration_t other);
extern duration_t monotonic_time(void);
