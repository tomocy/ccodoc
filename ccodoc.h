#pragma once

#include "time.h"
#include <stdbool.h>

typedef tick_timer_t action_t;

typedef void (*event_listener_t)(void*);

typedef struct {
    void* listener;
    event_listener_t listen;
} event_t;

typedef enum {
    holding_water,
    releasing_water,
} water_flow_state_t;

// kakehi（筧）
typedef struct {
    water_flow_state_t state;

    bool disabled;

    action_t holding_water;

    unsigned int release_water_amount;
    action_t releasing_water;
} kakehi_t;

// tsutsu（筒）
typedef struct {
    water_flow_state_t state;

    unsigned int water_amount;
    unsigned int water_capacity;
    action_t releasing_water;

    event_t on_got_drip;
    event_t on_bumped;
} tsutsu_t;

// hachi（手水鉢）
typedef struct {
    water_flow_state_t state;
    action_t releasing_water;
} hachi_t;

// ccodoc（鹿威し）
typedef struct {
    kakehi_t kakehi;
    tsutsu_t tsutsu;
    hachi_t hachi;
} ccodoc_t;

extern void tick_ccodoc(ccodoc_t* ccodoc, duration_t delta);

extern float tsutsu_water_amount_ratio(const tsutsu_t* tsutsu);

extern void tick_action(action_t* action, duration_t delta);
extern void reset_action(action_t* action);
extern float action_progress_ratio(const action_t* action);
extern bool action_has_finished(const action_t* action);

extern void notify_listener(const event_t* event);
