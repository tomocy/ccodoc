#include "ccodoc.h"

static void ccodoc_tick_kakehi(ccodoc* ccodoc, const duration delta);
static void ccodoc_tick_tsutsu(ccodoc* ccodoc, const duration delta);
static void ccodoc_tick_hachi(ccodoc* ccodoc, const duration delta);
static void ccodoc_pour_tsutsu_by(ccodoc_tsutsu* tsutsu, float ratio);
static void ccodoc_set_tsutsu_water_amount_by(ccodoc_tsutsu* tsutsu, float ratio);
static void ccodoc_shift_hachi_state(ccodoc_hachi* hachi, ccodoc_water_flow_state state);

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
        ticker_tick(&kakehi->holding_timer.ticker, delta);

        if (!timer_is_timeout(&kakehi->holding_timer)) {
            return;
        }

        kakehi->state = ccodoc_releasing_water;
        ticker_reset(&kakehi->releasing_timer.ticker);

        ccodoc_pour_tsutsu_by(&ccodoc->tsutsu, kakehi->pour_ratio);

        break;
    case ccodoc_releasing_water:
        ticker_tick(&kakehi->releasing_timer.ticker, delta);

        if (!timer_is_timeout(&kakehi->releasing_timer)) {
            return;
        }

        kakehi->state = ccodoc_holding_water;
        ticker_reset(&kakehi->holding_timer.ticker);

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

        tsutsu->state = ccodoc_releasing_water;
        ticker_reset(&tsutsu->releasing_timer.ticker);

        break;
    case ccodoc_releasing_water: {
        ticker_tick(&tsutsu->releasing_timer.ticker, delta);

        float releasing_ratio = timer_timeout_ratio(&tsutsu->releasing_timer);

        ccodoc_set_tsutsu_water_amount_by(tsutsu, 1 - releasing_ratio);

        ccodoc_shift_hachi_state(&ccodoc->hachi, ccodoc_releasing_water);

        if (releasing_ratio >= 1) {
            tsutsu->state = ccodoc_holding_water;
            tsutsu->water_amount = 0;
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
        ticker_tick(&hachi->releasing_timer.ticker, delta);

        if (!timer_is_timeout(&hachi->releasing_timer)) {
            break;
        }

        ccodoc_shift_hachi_state(hachi, ccodoc_holding_water);

        break;
    }
}

static void ccodoc_pour_tsutsu_by(ccodoc_tsutsu* tsutsu, float ratio)
{
    unsigned int delta = (unsigned int)((float)tsutsu->water_capacity * ratio);
    tsutsu->water_amount = (tsutsu->water_capacity - tsutsu->water_amount > delta)
        ? tsutsu->water_amount + delta
        : tsutsu->water_capacity;
}

static void ccodoc_set_tsutsu_water_amount_by(ccodoc_tsutsu* tsutsu, float ratio)
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

static void ccodoc_shift_hachi_state(ccodoc_hachi* hachi, ccodoc_water_flow_state state)
{
    if (hachi->state == state) {
        return;
    }

    hachi->state = state;

    switch (hachi->state) {
    case ccodoc_releasing_water:
        ticker_reset(&hachi->releasing_timer.ticker);
        break;
    default:
        break;
    }
}
