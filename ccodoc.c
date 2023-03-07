#include "ccodoc.h"

// kakehi
static void ccodoc_kakehi_tick_holding(ccodoc_kakehi* kakehi, int delta_msec);
static void ccodoc_kakehi_tick_pouring(ccodoc_kakehi* kakehi, int delta_msec);

// tsutsu
static void ccodoc_tsutsu_pour(ccodoc_tsutsu* tsutsu, int amount);
static void ccodoc_tsutsu_release(ccodoc_tsutsu* tsutsu, int amount);

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
            ccodoc_tsutsu_pour(tsutsu, 1);
        }

        if (ccodoc_tsutsu_holding_ratio(tsutsu) < 1) {
            return;
        }

        tsutsu->state = ccodoc_tsutsu_state_releasing;

        break;
    }
    case ccodoc_tsutsu_state_releasing:
        ccodoc_tsutsu_release(tsutsu, 2);
        break;
    }
}

static void ccodoc_kakehi_tick_holding(ccodoc_kakehi* kakehi, int delta_msec)
{
    ticker_tick(&kakehi->holding_timer.ticker, delta_msec);

    if (!timer_is_timeout(&kakehi->holding_timer)) {
        return;
    }

    kakehi->state = ccodoc_kakehi_state_pouring;
    ticker_reset(&kakehi->pouring_timer.ticker);
}

static void ccodoc_kakehi_tick_pouring(ccodoc_kakehi* kakehi, int delta_msec)
{
    ticker_tick(&kakehi->pouring_timer.ticker, delta_msec);

    if (!timer_is_timeout(&kakehi->pouring_timer)) {
        return;
    }

    kakehi->state = ccodoc_kakehi_state_holding;
    ticker_reset(&kakehi->holding_timer.ticker);
}

static void ccodoc_tsutsu_pour(ccodoc_tsutsu* tsutsu, int amount)
{
    tsutsu->amount += amount;
    if (tsutsu->amount < tsutsu->capacity) {
        return;
    }

    tsutsu->state = ccodoc_tsutsu_state_releasing;
    tsutsu->amount = tsutsu->capacity;
}

static void ccodoc_tsutsu_release(ccodoc_tsutsu* tsutsu, int amount)
{
    tsutsu->amount -= amount;
    if (tsutsu->amount > 0) {
        return;
    }

    tsutsu->state = ccodoc_tsutsu_state_holding;
    tsutsu->amount = 0;
}

float ccodoc_tsutsu_holding_ratio(const ccodoc_tsutsu* tsutsu)
{
    assert(tsutsu->capacity != 0);
    return (float)tsutsu->amount / tsutsu->capacity;
}
