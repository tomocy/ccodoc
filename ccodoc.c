#include "ccodoc.h"

#include "math.h"
#include "time.h"
#include <assert.h>
#include <stddef.h>

static void tick_kakehi(ccodoc_t* ccodoc, duration_t delta);
static void tick_tsutsu(ccodoc_t* ccodoc, duration_t delta);
static void tick_hachi(ccodoc_t* ccodoc, duration_t delta);

static void hold_water_kakehi(ccodoc_t* ccodoc);
static void release_water_kakehi(ccodoc_t* ccodoc);
static void carry_time_kakehi(ccodoc_t* ccodoc, const action_t* prev_action);

static void hold_water_tsutsu(ccodoc_t* ccodoc);
static void release_water_tsutsu(ccodoc_t* ccodoc);

static void hold_water_hachi(ccodoc_t* ccodoc);
static void release_water_hachi(ccodoc_t* ccodoc);

static void drip_water_into_tsutsu(tsutsu_t* tsutsu, unsigned int amount);

void tick_ccodoc(ccodoc_t* const ccodoc, const duration_t delta)
{
    tick_kakehi(ccodoc, delta);
    tick_tsutsu(ccodoc, delta);
    tick_hachi(ccodoc, delta);
}

// NOLINTNEXTLINE(misc-no-recursion)
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

        release_water_kakehi(ccodoc);

        carry_time_kakehi(ccodoc, &kakehi->holding_water);

        break;
    case releasing_water:
        tick_action(&kakehi->releasing_water, delta);

        if (!action_has_finished(&kakehi->releasing_water)) {
            return;
        }

        hold_water_kakehi(ccodoc);

        carry_time_kakehi(ccodoc, &kakehi->releasing_water);

        break;
    }
}

// NOLINTNEXTLINE(misc-no-recursion)
static void carry_time_kakehi(ccodoc_t* const ccodoc, const action_t* const prev_action)
{
    const duration_t overflowed = overflow_time(prev_action);
    if (overflowed.msecs == 0) {
        return;
    }

    tick_kakehi(ccodoc, overflowed);
}

static void tick_tsutsu(ccodoc_t* const ccodoc, const duration_t delta)
{
    tsutsu_t* const tsutsu = &ccodoc->tsutsu;

    switch (ccodoc->tsutsu.state) {
    case holding_water:
        if (tsutsu_water_amount_ratio(tsutsu) < 1) {
            break;
        }

        release_water_tsutsu(ccodoc);

        break;
    case releasing_water: {
        tick_action(&tsutsu->releasing_water, delta);

        if (!action_has_finished(&tsutsu->releasing_water)) {
            break;
        }

        notify_listener(&tsutsu->on_bumped);

        hold_water_tsutsu(ccodoc);

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

        hold_water_hachi(ccodoc);

        break;
    }
}

static void hold_water_kakehi(ccodoc_t* const ccodoc)
{
    kakehi_t* const kakehi = &ccodoc->kakehi;

    const water_flow_state_t state = holding_water;
    if (kakehi->state == state) {
        return;
    }

    kakehi->state = state;
    reset_action(&kakehi->holding_water);
}

static void release_water_kakehi(ccodoc_t* const ccodoc)
{
    kakehi_t* const kakehi = &ccodoc->kakehi;

    const water_flow_state_t state = releasing_water;
    if (kakehi->state == state) {
        return;
    }

    kakehi->state = state;
    reset_action(&kakehi->releasing_water);

    drip_water_into_tsutsu(&ccodoc->tsutsu, kakehi->release_water_amount);
}

static void hold_water_tsutsu(ccodoc_t* const ccodoc)
{
    tsutsu_t* const tsutsu = &ccodoc->tsutsu;

    const water_flow_state_t state = holding_water;
    if (tsutsu->state == state) {
        return;
    }

    tsutsu->state = state;
    tsutsu->water_amount = 0;
}

static void release_water_tsutsu(ccodoc_t* const ccodoc)
{
    tsutsu_t* const tsutsu = &ccodoc->tsutsu;

    const water_flow_state_t state = releasing_water;
    if (tsutsu->state == state) {
        return;
    }

    tsutsu->state = state;
    reset_action(&tsutsu->releasing_water);

    tsutsu->water_amount = 0;

    release_water_hachi(ccodoc);
}

static void hold_water_hachi(ccodoc_t* const ccodoc)
{
    hachi_t* const hachi = &ccodoc->hachi;

    const water_flow_state_t state = holding_water;
    if (hachi->state == state) {
        return;
    }

    hachi->state = state;
}

static void release_water_hachi(ccodoc_t* const ccodoc)
{
    hachi_t* const hachi = &ccodoc->hachi;

    const water_flow_state_t state = releasing_water;
    if (hachi->state == state) {
        return;
    }

    hachi->state = state;
    reset_action(&hachi->releasing_water);
}

static void drip_water_into_tsutsu(tsutsu_t* const tsutsu, const unsigned int amount)
{
    tsutsu->water_amount = MIN(tsutsu->water_amount + amount, tsutsu->water_capacity);
    notify_listener(&tsutsu->on_got_drip);
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
