#pragma once

#include <assert.h>
#include <curses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

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

typedef struct {
    bool help;
    bool debug;
    bool test;

    duration duration;
} ccodoc_context;

typedef enum {
    ccodoc_holding_water,
    ccodoc_releasing_water,
} ccodoc_water_flow_state;

// kakehi（筧）
typedef struct {
    ccodoc_water_flow_state state;
    float release_water_ratio;
    timer holding_water_timer;
    timer releasing_water_timer;
} ccodoc_kakehi;

// tsutsu（筒）
typedef struct {
    ccodoc_water_flow_state state;
    unsigned int water_amount;
    unsigned int water_capacity;
    timer releasing_water_timer;
} ccodoc_tsutsu;

// hachi（手水鉢）
typedef struct {
    ccodoc_water_flow_state state;
    timer releasing_water_timer;
} ccodoc_hachi;

// ccodoc（鹿威し）
typedef struct {
    ccodoc_kakehi kakehi;
    ccodoc_tsutsu tsutsu;
    ccodoc_hachi hachi;
} ccodoc;

typedef struct {
    WINDOW* window;
} ccodoc_renderer;

// ccodoc.c
extern void ccodoc_tick(ccodoc* ccodoc, const duration delta);
// - debug
extern float ccodoc_tsutsu_water_amount_ratio(const ccodoc_tsutsu* tsutsu);
// - test
extern int test_ccodoc(void);

// renderer.c
extern void ccodoc_init_renderer(ccodoc_renderer* renderer);
extern void ccodoc_deinit_renderer(ccodoc_renderer* renderer);
extern void ccodoc_render(ccodoc_renderer* renderer, const ccodoc_context* ctx, const timer* timer, const ccodoc* ccodoc);

// string.c
extern bool str_equals_to(const char* str, const char* other);
// - test
extern int test_str(void);

// time.c
// - timer
extern void timer_tick(timer* timer, const duration delta);
extern void timer_reset(timer* timer);
extern bool timer_is_timeout(const timer* timer);
extern float timer_timeout_ratio(const timer* timer);
extern duration timer_remaining_time(const timer* timer);
// - sleep
extern void sleep_for(const duration duration);
// - moment
extern moment moment_from_duration(const duration duration, time_precision precision);
// - duration
extern duration duration_from_moment(const moment moment);
// - test
extern int test_time(void);

// test
#define EXPECT(actual, expected)               \
    {                                          \
        const bool x = (actual) == (expected); \
        printf("%s: ", x ? "PASS" : "FAIL");   \
        printf(#actual " == " #expected);      \
        printf("\n");                          \
        if (!x) {                              \
            return EXIT_FAILURE;               \
        }                                      \
    }

#define EXPECT_TRUE(actual) EXPECT(actual, true)
#define EXPECT_FALSE(actual) EXPECT(actual, false)
