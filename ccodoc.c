#include "ccodoc.h"

// kakehi
static void ccodoc_kakehi_tick_holding(ccodoc_kakehi* kakehi, int delta_msec);
static void ccodoc_kakehi_tick_pouring(ccodoc_kakehi* kakehi, int delta_msec);

// tsutsu
static void ccodoc_tsutsu_pour_water(ccodoc_tsutsu* tsutsu, int water_amount);
static void ccodoc_tsutsu_release_water(ccodoc_tsutsu* tsutsu, int water_amount);

void ccodoc_tick(ccodoc* ccodoc, int delta_msec)
{
    ccodoc_kakehi* kakehi = &ccodoc->kakehi;
    switch (kakehi->state) {
    case ccodoc_kakehi_state_holding:
        ccodoc_kakehi_tick_holding(&ccodoc->kakehi, delta_msec);
        break;
    case ccodoc_kakehi_state_pouring:
        ccodoc_kakehi_tick_pouring(&ccodoc->kakehi, delta_msec);
        break;
    }

    ccodoc_tsutsu* tsutsu = &ccodoc->tsutsu;
    switch (tsutsu->state) {
    case ccodoc_tsutsu_state_holding: {
        if (kakehi->state == ccodoc_kakehi_state_pouring) {
            ccodoc_tsutsu_pour_water(tsutsu, 1);
        }

        if (ccodoc_tsutsu_holding_water_ratio(tsutsu) < 1) {
            return;
        }

        tsutsu->state = ccodoc_tsutsu_state_releasing;

        break;
    }
    case ccodoc_tsutsu_state_releasing:
        ccodoc_tsutsu_release_water(tsutsu, 2);
        break;
    }
}

static void ccodoc_kakehi_tick_holding(ccodoc_kakehi* kakehi, int delta_msec)
{
    ticker_tick(&kakehi->holding_ticker, delta_msec);

    float holding_ratio = (float)ticker_elapsed_msec(&kakehi->holding_ticker)
        / kakehi->holding_period_msec;

    if (holding_ratio < 1) {
        return;
    }

    kakehi->state = ccodoc_kakehi_state_pouring;
    ticker_reset(&kakehi->pouring_ticker);
}

static void ccodoc_kakehi_tick_pouring(ccodoc_kakehi* kakehi, int delta_msec)
{
    ticker_tick(&kakehi->pouring_ticker, delta_msec);

    float pouring_ratio = (float)ticker_elapsed_msec(&kakehi->pouring_ticker)
        / kakehi->pouring_period_msec;

    if (pouring_ratio < 1) {
        return;
    }

    kakehi->state = ccodoc_kakehi_state_holding;
    ticker_reset(&kakehi->holding_ticker);
}

static void ccodoc_tsutsu_pour_water(ccodoc_tsutsu* tsutsu, int water_amount)
{
    if (tsutsu->state != ccodoc_tsutsu_state_holding) {
        return;
    }

    tsutsu->water_amount += water_amount;
    if (tsutsu->water_amount < tsutsu->water_capacity) {
        return;
    }

    tsutsu->state = ccodoc_tsutsu_state_releasing;
    tsutsu->water_amount = tsutsu->water_capacity;
}

static void ccodoc_tsutsu_release_water(ccodoc_tsutsu* tsutsu, int water_amount)
{
    if (tsutsu->state != ccodoc_tsutsu_state_releasing) {
        return;
    }

    tsutsu->water_amount -= water_amount;
    if (tsutsu->water_amount > 0) {
        return;
    }

    tsutsu->state = ccodoc_tsutsu_state_holding;
    tsutsu->water_amount = 0;
}

float ccodoc_tsutsu_holding_water_ratio(const ccodoc_tsutsu* tsutsu)
{
    assert(tsutsu->water_capacity != 0);
    return (float)tsutsu->water_amount / tsutsu->water_capacity;
}
