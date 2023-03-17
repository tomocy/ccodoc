#include "ccodoc.h"

static void ccodoc_tick_kakehi(ccodoc* ccodoc, const duration delta);
static void ccodoc_tick_tsutsu(ccodoc* ccodoc, const duration delta);
static void ccodoc_tick_hachi(ccodoc* ccodoc, const duration delta);

static void ccodoc_hold_kakehi_water(ccodoc* ccodoc);
static void ccodoc_release_kakehi_water(ccodoc* ccodoc);

static void ccodoc_hold_tsutsu_water(ccodoc* ccodoc);
static void ccodoc_release_tsutsu_water(ccodoc* ccodoc);

static void ccodoc_hold_hachi_water(ccodoc* ccodoc);
static void ccodoc_release_hachi_water(ccodoc* ccodoc);

static void ccodoc_pour_tsutsu_by(ccodoc_tsutsu* tsutsu, float ratio);
static void ccodoc_fill_tsutsu_with_water_by(ccodoc_tsutsu* tsutsu, float ratio);

void ccodoc_tick(ccodoc* ccodoc, const duration delta)
{
    ccodoc_tick_kakehi(ccodoc, delta);

    ccodoc_tick_tsutsu(ccodoc, delta);

    ccodoc_tick_hachi(ccodoc, delta);
}

static void ccodoc_tick_kakehi(ccodoc* ccodoc, const duration delta)
{
    ccodoc_kakehi* kakehi = &ccodoc->kakehi;

    switch (kakehi->state) {
    case ccodoc_holding_water:
        timer_tick(&kakehi->holding_water_timer, delta);

        if (!timer_is_timeout(&kakehi->holding_water_timer)) {
            return;
        }

        ccodoc_release_kakehi_water(ccodoc);

        break;
    case ccodoc_releasing_water:
        timer_tick(&kakehi->releasing_water_timer, delta);

        if (!timer_is_timeout(&kakehi->releasing_water_timer)) {
            return;
        }

        ccodoc_hold_kakehi_water(ccodoc);

        break;
    }
}

static void ccodoc_tick_tsutsu(ccodoc* ccodoc, const duration delta)
{
    ccodoc_tsutsu* tsutsu = &ccodoc->tsutsu;

    switch (ccodoc->tsutsu.state) {
    case ccodoc_holding_water:
        if (ccodoc_tsutsu_water_amount_ratio(tsutsu) < 1) {
            break;
        }

        ccodoc_release_tsutsu_water(ccodoc);

        break;
    case ccodoc_releasing_water: {
        timer_tick(&tsutsu->releasing_water_timer, delta);

        float water_ratio = timer_timeout_ratio(&tsutsu->releasing_water_timer);

        ccodoc_fill_tsutsu_with_water_by(tsutsu, 1.0f - water_ratio);

        if (water_ratio >= 1) {
            ccodoc_hold_tsutsu_water(ccodoc);
        }

        break;
    }
    }
}

static void ccodoc_tick_hachi(ccodoc* ccodoc, const duration delta)
{
    ccodoc_hachi* hachi = &ccodoc->hachi;

    switch (ccodoc->hachi.state) {
    case ccodoc_holding_water:
        break;
    case ccodoc_releasing_water:
        timer_tick(&hachi->releasing_water_timer, delta);

        if (!timer_is_timeout(&hachi->releasing_water_timer)) {
            break;
        }

        ccodoc_hold_hachi_water(ccodoc);

        break;
    }
}

static void ccodoc_hold_kakehi_water(ccodoc* ccodoc)
{
    ccodoc_kakehi* kakehi = &ccodoc->kakehi;

    ccodoc_water_flow_state state = ccodoc_holding_water;
    if (kakehi->state == state) {
        return;
    }

    kakehi->state = state;
    timer_reset(&kakehi->holding_water_timer);
}

static void ccodoc_release_kakehi_water(ccodoc* ccodoc)
{
    ccodoc_kakehi* kakehi = &ccodoc->kakehi;

    ccodoc_water_flow_state state = ccodoc_releasing_water;
    if (kakehi->state == state) {
        return;
    }

    kakehi->state = state;
    timer_reset(&kakehi->releasing_water_timer);

    ccodoc_pour_tsutsu_by(&ccodoc->tsutsu, kakehi->release_water_ratio);
}

static void ccodoc_hold_tsutsu_water(ccodoc* ccodoc)
{
    ccodoc_tsutsu* tsutsu = &ccodoc->tsutsu;

    ccodoc_water_flow_state state = ccodoc_holding_water;
    if (tsutsu->state == state) {
        return;
    }

    tsutsu->state = state;
    tsutsu->water_amount = 0;
}

static void ccodoc_release_tsutsu_water(ccodoc* ccodoc)
{
    ccodoc_tsutsu* tsutsu = &ccodoc->tsutsu;

    ccodoc_water_flow_state state = ccodoc_releasing_water;
    if (tsutsu->state == state) {
        return;
    }

    tsutsu->state = state;
    timer_reset(&tsutsu->releasing_water_timer);

    ccodoc_release_hachi_water(ccodoc);
}

static void ccodoc_hold_hachi_water(ccodoc* ccodoc)
{
    ccodoc_hachi* hachi = &ccodoc->hachi;

    ccodoc_water_flow_state state = ccodoc_holding_water;
    if (hachi->state == state) {
        return;
    }

    hachi->state = state;
}

static void ccodoc_release_hachi_water(ccodoc* ccodoc)
{
    ccodoc_hachi* hachi = &ccodoc->hachi;

    ccodoc_water_flow_state state = ccodoc_releasing_water;
    if (hachi->state == state) {
        return;
    }

    hachi->state = state;
    timer_reset(&hachi->releasing_water_timer);
}

static void ccodoc_pour_tsutsu_by(ccodoc_tsutsu* tsutsu, float ratio)
{
    unsigned int delta = (unsigned int)((float)tsutsu->water_capacity * ratio);
    tsutsu->water_amount = (tsutsu->water_capacity - tsutsu->water_amount > delta)
        ? tsutsu->water_amount + delta
        : tsutsu->water_capacity;
}

static void ccodoc_fill_tsutsu_with_water_by(ccodoc_tsutsu* tsutsu, float ratio)
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

float ccodoc_tsutsu_water_amount_ratio(const ccodoc_tsutsu* tsutsu)
{
    assert(tsutsu->water_capacity != 0);
    return (float)tsutsu->water_amount / (float)tsutsu->water_capacity;
}

void test_ccodoc(void)
{
    ccodoc ccodoc = {
        .kakehi = {
            .release_water_ratio = 0.5f,
            .holding_water_timer = {
                .duration = { .msec = 2500 },
            },
            .releasing_water_timer = {
                .duration = { .msec = 500 },
            },
        },
        .tsutsu = {
            .water_capacity = 10,
            .releasing_water_timer = {
                .duration = { .msec = 1500 },
            },
        },
        .hachi = {
            .releasing_water_timer = {
                .duration = { .msec = 1000 },
            },
        },
    };

    {
        printf("- initial\n");

        EXPECT(ccodoc.kakehi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.holding_water_timer), 0);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.releasing_water_timer), 0);

        EXPECT(ccodoc.tsutsu.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT(ccodoc_tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0);

        EXPECT(ccodoc.hachi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.hachi.releasing_water_timer), 0);
    }

    {
        printf("- tick (250ms)\n");
        ccodoc_tick(&ccodoc, (duration) { .msec = 250 });

        EXPECT(ccodoc.kakehi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.holding_water_timer), 0.1f);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.releasing_water_timer), 0);

        EXPECT(ccodoc.tsutsu.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT(ccodoc_tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0);

        EXPECT(ccodoc.hachi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.hachi.releasing_water_timer), 0);
    }

    {
        printf("- tick (1750ms)\n");
        ccodoc_tick(&ccodoc, (duration) { .msec = 1750 });

        EXPECT(ccodoc.kakehi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.holding_water_timer), 0.8f);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.releasing_water_timer), 0);

        EXPECT(ccodoc.tsutsu.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT(ccodoc_tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0);

        EXPECT(ccodoc.hachi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.hachi.releasing_water_timer), 0);
    }

    {
        printf("- tick (1000ms)\n");
        ccodoc_tick(&ccodoc, (duration) { .msec = 1000 });

        EXPECT(ccodoc.kakehi.state, ccodoc_releasing_water);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.holding_water_timer), 1.2f);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.releasing_water_timer), 0);

        EXPECT(ccodoc.tsutsu.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT(ccodoc_tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0.5f);

        EXPECT(ccodoc.hachi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.hachi.releasing_water_timer), 0);
    }

    {
        printf("- tick (250ms)\n");
        ccodoc_tick(&ccodoc, (duration) { .msec = 250 });

        EXPECT(ccodoc.kakehi.state, ccodoc_releasing_water);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.holding_water_timer), 1.2f);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.releasing_water_timer), 0.5f);

        EXPECT(ccodoc.tsutsu.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT(ccodoc_tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0.5f);

        EXPECT(ccodoc.hachi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.hachi.releasing_water_timer), 0);
    }

    {
        printf("- tick (250ms)\n");
        ccodoc_tick(&ccodoc, (duration) { .msec = 250 });

        EXPECT(ccodoc.kakehi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.holding_water_timer), 0);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.releasing_water_timer), 1);

        EXPECT(ccodoc.tsutsu.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT(ccodoc_tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0.5f);

        EXPECT(ccodoc.hachi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.hachi.releasing_water_timer), 0);
    }

    {
        printf("- tick (2500ms)\n");
        ccodoc_tick(&ccodoc, (duration) { .msec = 2500 });

        EXPECT(ccodoc.kakehi.state, ccodoc_releasing_water);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.holding_water_timer), 1);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.releasing_water_timer), 0);

        EXPECT(ccodoc.tsutsu.state, ccodoc_releasing_water);
        EXPECT(timer_timeout_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT(ccodoc_tsutsu_water_amount_ratio(&ccodoc.tsutsu), 1);

        EXPECT(ccodoc.hachi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.hachi.releasing_water_timer), 2.5);
    }

    {
        printf("- tick (500ms)\n");
        ccodoc_tick(&ccodoc, (duration) { .msec = 500 });

        EXPECT(ccodoc.kakehi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.holding_water_timer), 0);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.releasing_water_timer), 1);

        EXPECT(ccodoc.tsutsu.state, ccodoc_releasing_water);
        EXPECT(timer_timeout_ratio(&ccodoc.tsutsu.releasing_water_timer), 1.0f / 3);
        EXPECT(ccodoc_tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0.6f);

        EXPECT(ccodoc.hachi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.hachi.releasing_water_timer), 2.5);
    }

    {
        printf("- tick (1000ms)\n");
        ccodoc_tick(&ccodoc, (duration) { .msec = 1000 });

        EXPECT(ccodoc.kakehi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.holding_water_timer), 0.4f);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.releasing_water_timer), 1);

        EXPECT(ccodoc.tsutsu.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.tsutsu.releasing_water_timer), 1);
        EXPECT(ccodoc_tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0);

        EXPECT(ccodoc.hachi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.hachi.releasing_water_timer), 2.5);
    }
}
