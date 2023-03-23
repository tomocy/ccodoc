#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

// platform
#define PLATFORM_UNKNOWN 0
#define PLATFORM_LINUX 1
#define PLATFORM_MACOS 2

// math
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(min, max, x) MIN(MAX(x, min), max)

typedef void (*void_callback)(void);

// time
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

// string
extern bool str_equals(const char* str, const char* other);
extern bool str_equals_n(const char* str, const char* other, unsigned int n);
extern size_t decode_char_utf8(const char* src);

// engine
typedef struct {
    bool decorative;
    duration duration;

    bool help;
    bool debug;
} context;

typedef enum {
    holding_water,
    releasing_water,
} water_flow_state;

// kakehi（筧）
typedef struct {
    water_flow_state state;
    float release_water_ratio;
    timer holding_water_timer;
    timer releasing_water_timer;
} kakehi;

// tsutsu（筒）
typedef struct {
    water_flow_state state;
    unsigned int water_amount;
    unsigned int water_capacity;
    timer releasing_water_timer;

    void_callback on_poured;
    void_callback on_released_water;
} tsutsu;

// hachi（手水鉢）
typedef struct {
    water_flow_state state;
    timer releasing_water_timer;
} hachi;

// ccodoc（鹿威し）
typedef struct {
    kakehi kakehi;
    tsutsu tsutsu;
    hachi hachi;
} ccodoc;

extern void tick_ccodoc(ccodoc* ccodoc, const duration delta);
// - debug
extern float tsutsu_water_amount_ratio(const tsutsu* tsutsu);
