#include "ccodoc.h"

#include "math.h"
#include "time.h"
#include <assert.h>
#include <stddef.h>

static void tick_kakehi(struct ccodoc* ccodoc, struct duration delta);
static void tick_tsutsu(struct ccodoc* ccodoc, struct duration delta);
static void tick_hachi(struct ccodoc* ccodoc, struct duration delta);

static void hold_water_kakehi(struct ccodoc* ccodoc);
static void release_water_kakehi(struct ccodoc* ccodoc);

static void hold_water_tsutsu(struct ccodoc* ccodoc);
static void release_water_tsutsu(struct ccodoc* ccodoc);

static void hold_water_hachi(struct ccodoc* ccodoc);
static void release_water_hachi(struct ccodoc* ccodoc);

static void drip_water_into_tsutsu(struct tsutsu* tsutsu, unsigned int amount);

void tick_ccodoc(struct ccodoc* const ccodoc, const struct duration delta)
{
    tick_kakehi(ccodoc, delta);
    tick_tsutsu(ccodoc, delta);
    tick_hachi(ccodoc, delta);
}

static void tick_kakehi(struct ccodoc* const ccodoc, struct duration delta)
{
    struct kakehi* const kakehi = &ccodoc->kakehi;

    if (kakehi->disabled) {
        return;
    }

    static struct duration carried_delta = { 0 };

    delta.msecs += carried_delta.msecs;
    carried_delta.msecs = 0;

    switch (kakehi->state) {
    case holding_water:
        tick_action(&kakehi->holding_water, delta);

        if (!action_has_finished(&kakehi->holding_water)) {
            return;
        }

        release_water_kakehi(ccodoc);

        carried_delta = get_overflow_time(&kakehi->holding_water);

        break;
    case releasing_water:
        tick_action(&kakehi->releasing_water, delta);

        if (!action_has_finished(&kakehi->releasing_water)) {
            return;
        }

        hold_water_kakehi(ccodoc);

        carried_delta = get_overflow_time(&kakehi->releasing_water);

        break;
    }
}

static void tick_tsutsu(struct ccodoc* const ccodoc, const struct duration delta)
{
    struct tsutsu* const tsutsu = &ccodoc->tsutsu;

    switch (tsutsu->state) {
    case holding_water:
        if (get_tsutsu_water_amount_ratio(tsutsu) < 1) {
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

static void tick_hachi(struct ccodoc* const ccodoc, const struct duration delta)
{
    struct hachi* const hachi = &ccodoc->hachi;

    switch (hachi->state) {
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

static void hold_water_kakehi(struct ccodoc* const ccodoc)
{
    struct kakehi* const kakehi = &ccodoc->kakehi;

    const enum water_flow_state state = holding_water;
    if (kakehi->state == state) {
        return;
    }

    kakehi->state = state;
    reset_action(&kakehi->holding_water);
}

static void release_water_kakehi(struct ccodoc* const ccodoc)
{
    struct kakehi* const kakehi = &ccodoc->kakehi;

    const enum water_flow_state state = releasing_water;
    if (kakehi->state == state) {
        return;
    }

    kakehi->state = state;
    reset_action(&kakehi->releasing_water);

    drip_water_into_tsutsu(&ccodoc->tsutsu, kakehi->release_water_amount);
}

static void hold_water_tsutsu(struct ccodoc* const ccodoc)
{
    struct tsutsu* const tsutsu = &ccodoc->tsutsu;

    const enum water_flow_state state = holding_water;
    if (tsutsu->state == state) {
        return;
    }

    tsutsu->state = state;
    tsutsu->water_amount = 0;
}

static void release_water_tsutsu(struct ccodoc* const ccodoc)
{
    struct tsutsu* const tsutsu = &ccodoc->tsutsu;

    const enum water_flow_state state = releasing_water;
    if (tsutsu->state == state) {
        return;
    }

    tsutsu->state = state;
    reset_action(&tsutsu->releasing_water);

    tsutsu->water_amount = 0;

    release_water_hachi(ccodoc);
}

static void hold_water_hachi(struct ccodoc* const ccodoc)
{
    struct hachi* const hachi = &ccodoc->hachi;

    const enum water_flow_state state = holding_water;
    if (hachi->state == state) {
        return;
    }

    hachi->state = state;
}

static void release_water_hachi(struct ccodoc* const ccodoc)
{
    struct hachi* const hachi = &ccodoc->hachi;

    const enum water_flow_state state = releasing_water;
    if (hachi->state == state) {
        return;
    }

    hachi->state = state;
    reset_action(&hachi->releasing_water);
}

static void drip_water_into_tsutsu(struct tsutsu* const tsutsu, const unsigned int amount)
{
    tsutsu->water_amount = MIN(tsutsu->water_amount + amount, tsutsu->water_capacity);
    notify_listener(&tsutsu->on_got_drip);
}

float get_tsutsu_water_amount_ratio(const struct tsutsu* const tsutsu)
{
    assert(tsutsu->water_capacity != 0);
    return (float)tsutsu->water_amount / (float)tsutsu->water_capacity;
}

void tick_action(action_t* const action, const struct duration delta)
{
    tick_timer(action, delta);
}

void reset_action(action_t* const action)
{
    reset_timer(action);
}

float get_action_progress_ratio(const action_t* const action)
{
    return get_elapsed_time_ratio(action);
}

bool action_has_finished(const action_t* const action)
{
    return timer_expires(action);
}

void notify_listener(const struct event* const event)
{
    if (event->listen == NULL) {
        return;
    }

    event->listen(event->listener);
}
