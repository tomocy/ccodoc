#include "ccodoc.h"

#include "math.h"
#include "time.h"
#include <assert.h>
#include <stddef.h>

static void tick_kakehi(ccodoc_t* ccodoc, duration_t delta);
static void tick_tsutsu(ccodoc_t* ccodoc, duration_t delta);
static void tick_hachi(ccodoc_t* ccodoc, duration_t delta);

static void hold_kakehi_water(ccodoc_t* ccodoc);
static void release_kakehi_water(ccodoc_t* ccodoc);

static void hold_tsutsu_water(ccodoc_t* ccodoc);
static void release_tsutsu_water(ccodoc_t* ccodoc);

static void hold_hachi_water(ccodoc_t* ccodoc);
static void release_hachi_water(ccodoc_t* ccodoc);

static void pour_tsutsu_by(tsutsu_t* tsutsu, float ratio);
static void fill_tsutsu_by(tsutsu_t* tsutsu, float ratio);

void tick_ccodoc(ccodoc_t* const ccodoc, const duration_t delta)
{
    tick_kakehi(ccodoc, delta);
    tick_tsutsu(ccodoc, delta);
    tick_hachi(ccodoc, delta);
}

static void tick_kakehi(ccodoc_t* const ccodoc, const duration_t delta)
{
    if (ccodoc->kakehi.disabled) {
        return;
    }

    kakehi_t* const kakehi = &ccodoc->kakehi;

    switch (kakehi->state) {
    case holding_water:
        tick_action(&kakehi->holding_water, delta);

        if (!action_has_finished(&kakehi->holding_water)) {
            return;
        }

        release_kakehi_water(ccodoc);

        break;
    case releasing_water:
        tick_action(&kakehi->releasing_water, delta);

        if (!action_has_finished(&kakehi->releasing_water)) {
            return;
        }

        hold_kakehi_water(ccodoc);

        break;
    }
}

static void tick_tsutsu(ccodoc_t* const ccodoc, const duration_t delta)
{
    tsutsu_t* const tsutsu = &ccodoc->tsutsu;

    switch (ccodoc->tsutsu.state) {
    case holding_water:
        if (tsutsu_water_amount_ratio(tsutsu) < 1) {
            break;
        }

        release_tsutsu_water(ccodoc);

        break;
    case releasing_water: {
        tick_action(&tsutsu->releasing_water, delta);

        const float ratio = action_progress_ratio(&tsutsu->releasing_water);

        fill_tsutsu_by(tsutsu, 1.0f - ratio);

        if (ratio < 1) {
            break;
        }

        notify_listener(&tsutsu->on_released_water);

        hold_tsutsu_water(ccodoc);

        break;
    }
    }
}

static void tick_hachi(ccodoc_t* const ccodoc, const duration_t delta)
{
    hachi_t* const hachi = &ccodoc->hachi;

    switch (ccodoc->hachi.state) {
    case holding_water:
        break;
    case releasing_water:
        tick_action(&hachi->releasing_water, delta);

        if (!action_has_finished(&hachi->releasing_water)) {
            break;
        }

        hold_hachi_water(ccodoc);

        break;
    }
}

static void hold_kakehi_water(ccodoc_t* const ccodoc)
{
    kakehi_t* const kakehi = &ccodoc->kakehi;

    const water_flow_state_t state = holding_water;
    if (kakehi->state == state) {
        return;
    }

    kakehi->state = state;
    reset_action(&kakehi->holding_water);
}

static void release_kakehi_water(ccodoc_t* const ccodoc)
{
    kakehi_t* const kakehi = &ccodoc->kakehi;

    const water_flow_state_t state = releasing_water;
    if (kakehi->state == state) {
        return;
    }

    kakehi->state = state;
    reset_action(&kakehi->releasing_water);

    pour_tsutsu_by(&ccodoc->tsutsu, kakehi->release_water_ratio);
}

static void hold_tsutsu_water(ccodoc_t* const ccodoc)
{
    tsutsu_t* const tsutsu = &ccodoc->tsutsu;

    const water_flow_state_t state = holding_water;
    if (tsutsu->state == state) {
        return;
    }

    tsutsu->state = state;
    tsutsu->water_amount = 0;
}

static void release_tsutsu_water(ccodoc_t* const ccodoc)
{
    tsutsu_t* const tsutsu = &ccodoc->tsutsu;

    const water_flow_state_t state = releasing_water;
    if (tsutsu->state == state) {
        return;
    }

    tsutsu->state = state;
    reset_action(&tsutsu->releasing_water);

    release_hachi_water(ccodoc);
}

static void hold_hachi_water(ccodoc_t* const ccodoc)
{
    hachi_t* const hachi = &ccodoc->hachi;

    const water_flow_state_t state = holding_water;
    if (hachi->state == state) {
        return;
    }

    hachi->state = state;
}

static void release_hachi_water(ccodoc_t* const ccodoc)
{
    hachi_t* const hachi = &ccodoc->hachi;

    const water_flow_state_t state = releasing_water;
    if (hachi->state == state) {
        return;
    }

    hachi->state = state;
    reset_action(&hachi->releasing_water);
}

static void pour_tsutsu_by(tsutsu_t* const tsutsu, const float ratio)
{
    const unsigned int delta = (unsigned int)((float)tsutsu->water_capacity * ratio);
    tsutsu->water_amount = (tsutsu->water_capacity - tsutsu->water_amount > delta)
        ? tsutsu->water_amount + delta
        : tsutsu->water_capacity;

    notify_listener(&tsutsu->on_poured);
}

static void fill_tsutsu_by(tsutsu_t* const tsutsu, const float ratio)
{
    tsutsu->water_amount = (unsigned int)((float)tsutsu->water_capacity * CLAMP(0, 1, ratio));
}

float tsutsu_water_amount_ratio(const tsutsu_t* const tsutsu)
{
    assert(tsutsu->water_capacity != 0);
    return (float)tsutsu->water_amount / (float)tsutsu->water_capacity;
}

void tick_action(action_t* const action, const duration_t delta)
{
    tick_timer(action, delta);
}

void reset_action(action_t* const action)
{
    reset_timer(action);
}

float action_progress_ratio(const action_t* const action)
{
    return elapsed_time_ratio(action);
}

bool action_has_finished(const action_t* const action)
{
    return timer_expires(action);
}

void notify_listener(const event_t* const event)
{
    if (event->listen == NULL) {
        return;
    }

    event->listen(event->listener);
}
