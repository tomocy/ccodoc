#include "engine.h"

#include <assert.h>
#include <stddef.h>

static void tick_kakehi(ccodoc_t* ccodoc, const duration_t delta);
static void tick_tsutsu(ccodoc_t* ccodoc, const duration_t delta);
static void tick_hachi(ccodoc_t* ccodoc, const duration_t delta);

static void hold_kakehi_water(ccodoc_t* ccodoc);
static void release_kakehi_water(ccodoc_t* ccodoc);

static void hold_tsutsu_water(ccodoc_t* ccodoc);
static void release_tsutsu_water(ccodoc_t* ccodoc);

static void hold_hachi_water(ccodoc_t* ccodoc);
static void release_hachi_water(ccodoc_t* ccodoc);

static void pour_tsutsu_by(tsutsu_t* tsutsu, float ratio);
static void fill_tsutsu_by(tsutsu_t* tsutsu, float ratio);

void tick_ccodoc(ccodoc_t* ccodoc, const duration_t delta)
{
    tick_kakehi(ccodoc, delta);

    tick_tsutsu(ccodoc, delta);

    tick_hachi(ccodoc, delta);
}

static void tick_kakehi(ccodoc_t* ccodoc, const duration_t delta)
{
    kakehi_t* kakehi = &ccodoc->kakehi;

    switch (kakehi->state) {
    case holding_water:
        tick_timer(&kakehi->holding_water_timer, delta);

        if (!is_timeout(&kakehi->holding_water_timer)) {
            return;
        }

        release_kakehi_water(ccodoc);

        break;
    case releasing_water:
        tick_timer(&kakehi->releasing_water_timer, delta);

        if (!is_timeout(&kakehi->releasing_water_timer)) {
            return;
        }

        hold_kakehi_water(ccodoc);

        break;
    }
}

static void tick_tsutsu(ccodoc_t* ccodoc, const duration_t delta)
{
    tsutsu_t* tsutsu = &ccodoc->tsutsu;

    switch (ccodoc->tsutsu.state) {
    case holding_water:
        if (tsutsu_water_amount_ratio(tsutsu) < 1) {
            break;
        }

        release_tsutsu_water(ccodoc);

        break;
    case releasing_water: {
        tick_timer(&tsutsu->releasing_water_timer, delta);

        const float ratio = elapsed_time_ratio(&tsutsu->releasing_water_timer);

        fill_tsutsu_by(tsutsu, 1.0f - ratio);

        if (ratio < 1) {
            break;
        }

        hold_tsutsu_water(ccodoc);

        if (tsutsu->on_released_water != NULL) {
            tsutsu->on_released_water();
        }

        break;
    }
    }
}

static void tick_hachi(ccodoc_t* ccodoc, const duration_t delta)
{
    hachi_t* hachi = &ccodoc->hachi;

    switch (ccodoc->hachi.state) {
    case holding_water:
        break;
    case releasing_water:
        tick_timer(&hachi->releasing_water_timer, delta);

        if (!is_timeout(&hachi->releasing_water_timer)) {
            break;
        }

        hold_hachi_water(ccodoc);

        break;
    }
}

static void hold_kakehi_water(ccodoc_t* ccodoc)
{
    kakehi_t* kakehi = &ccodoc->kakehi;

    const water_flow_state_t state = holding_water;
    if (kakehi->state == state) {
        return;
    }

    kakehi->state = state;
    reset_timer(&kakehi->holding_water_timer);
}

static void release_kakehi_water(ccodoc_t* ccodoc)
{
    kakehi_t* kakehi = &ccodoc->kakehi;

    const water_flow_state_t state = releasing_water;
    if (kakehi->state == state) {
        return;
    }

    kakehi->state = state;
    reset_timer(&kakehi->releasing_water_timer);

    pour_tsutsu_by(&ccodoc->tsutsu, kakehi->release_water_ratio);
}

static void hold_tsutsu_water(ccodoc_t* ccodoc)
{
    tsutsu_t* tsutsu = &ccodoc->tsutsu;

    const water_flow_state_t state = holding_water;
    if (tsutsu->state == state) {
        return;
    }

    tsutsu->state = state;
    tsutsu->water_amount = 0;
}

static void release_tsutsu_water(ccodoc_t* ccodoc)
{
    tsutsu_t* tsutsu = &ccodoc->tsutsu;

    const water_flow_state_t state = releasing_water;
    if (tsutsu->state == state) {
        return;
    }

    tsutsu->state = state;
    reset_timer(&tsutsu->releasing_water_timer);

    release_hachi_water(ccodoc);
}

static void hold_hachi_water(ccodoc_t* ccodoc)
{
    hachi_t* hachi = &ccodoc->hachi;

    const water_flow_state_t state = holding_water;
    if (hachi->state == state) {
        return;
    }

    hachi->state = state;
}

static void release_hachi_water(ccodoc_t* ccodoc)
{
    hachi_t* hachi = &ccodoc->hachi;

    const water_flow_state_t state = releasing_water;
    if (hachi->state == state) {
        return;
    }

    hachi->state = state;
    reset_timer(&hachi->releasing_water_timer);
}

static void pour_tsutsu_by(tsutsu_t* tsutsu, float ratio)
{
    const unsigned int delta = (unsigned int)((float)tsutsu->water_capacity * ratio);
    tsutsu->water_amount = (tsutsu->water_capacity - tsutsu->water_amount > delta)
        ? tsutsu->water_amount + delta
        : tsutsu->water_capacity;

    if (tsutsu->on_poured != NULL) {
        tsutsu->on_poured();
    }
}

static void fill_tsutsu_by(tsutsu_t* tsutsu, float ratio)
{
    float r = ratio;
    if (ratio < 0) {
        r = 0;
    }
    if (ratio > 1) {
        r = 1;
    }

    tsutsu->water_amount = (unsigned int)((float)tsutsu->water_capacity * r);
}

float tsutsu_water_amount_ratio(const tsutsu_t* tsutsu)
{
    assert(tsutsu->water_capacity != 0);
    return (float)tsutsu->water_amount / (float)tsutsu->water_capacity;
}
