#pragma once

#include "time.h"
#include <stdbool.h>

typedef void (*void_callback_t)(void*);

typedef struct {
    void* listener;
    void_callback_t notify;
} event_t;

typedef enum {
    holding_water,
    releasing_water,
} water_flow_state_t;

// kakehi（筧）
typedef struct {
    water_flow_state_t state;
    float release_water_ratio;
    tick_timer_t holding_water_timer;
    tick_timer_t releasing_water_timer;
} kakehi_t;

// tsutsu（筒）
typedef struct {
    water_flow_state_t state;
    unsigned int water_amount;
    unsigned int water_capacity;
    tick_timer_t releasing_water_timer;

    event_t on_poured;
    event_t on_released_water;
} tsutsu_t;

// hachi（手水鉢）
typedef struct {
    water_flow_state_t state;
    tick_timer_t releasing_water_timer;
} hachi_t;

// ccodoc（鹿威し）
typedef struct {
    kakehi_t kakehi;
    tsutsu_t tsutsu;
    hachi_t hachi;
} ccodoc_t;

extern void tick_ccodoc(ccodoc_t* ccodoc, const duration_t delta);
// - debug
extern float tsutsu_water_amount_ratio(const tsutsu_t* tsutsu);

extern void notify_listener(event_t* event);
