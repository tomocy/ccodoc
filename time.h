#pragma once

#define _POSIX_C_SOURCE 200809L

#include <stdbool.h>
#include <time.h>

typedef enum {
    time_msec = 1,
    time_sec = 1000 * time_msec,
    time_min = 60 * time_sec,
    time_hour = 60 * time_min,
} time_precision;

typedef struct {
    unsigned int hours;
    unsigned int mins;
    unsigned int secs;
    unsigned int msecs;
} moment;

typedef struct {
    unsigned long msecs;
} duration;

typedef struct {
    duration elapsed;
} ticker;

typedef struct {
    duration duration;
    ticker ticker;
} timer;

// - timer
extern void tick_timer(timer* timer, const duration delta);
extern void reset_timer(timer* timer);
extern bool is_timeout(const timer* timer);
extern float elapsed_time_ratio(const timer* timer);
extern duration remaining_time(const timer* timer);
// - sleep
extern void sleep_for(const duration duration);
// - moment
extern moment moment_from_duration(const duration duration, time_precision precision);
// - duration
extern duration duration_from_moment(const moment moment);
extern duration unix_time(void);
