#pragma once

#include "time.h"
#include <stdbool.h>

typedef struct timer action_t;

typedef void (*event_listener_t)(void*);

struct event {
    void* listener;
    event_listener_t listen;
};

enum water_flow_state {
    holding_water,
    releasing_water,
};

// kakehi（筧）
struct kakehi {
    enum water_flow_state state;

    bool disabled;

    action_t holding_water;

    unsigned int release_water_amount;
    action_t releasing_water;
};

// tsutsu（筒）
struct tsutsu {
    enum water_flow_state state;

    unsigned int water_amount;
    unsigned int water_capacity;
    action_t releasing_water;

    struct event on_got_drip;
    struct event on_bumped;
};

// hachi（鉢）
struct hachi {
    enum water_flow_state state;
    action_t releasing_water;
};

// ccodoc（鹿威し）
struct ccodoc {
    struct kakehi kakehi;
    struct tsutsu tsutsu;
    struct hachi hachi;
};

extern void tick_ccodoc(struct ccodoc* ccodoc, struct duration delta);

extern float get_tsutsu_water_amount_ratio(const struct tsutsu* tsutsu);

extern void tick_action(action_t* action, struct duration delta);
extern void reset_action(action_t* action);
extern float get_action_progress_ratio(const action_t* action);
extern bool action_has_finished(const action_t* action);

extern void notify_listener(const struct event* event);
