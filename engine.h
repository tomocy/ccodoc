#pragma once

#include "time.h"
#include <stdbool.h>

typedef void (*void_callback)(void);

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
