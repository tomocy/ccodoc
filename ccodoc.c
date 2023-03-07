#include "ccodoc.h"

// kakehi
static void ccodoc_tick_kakehi_holding(ccodoc* ccodoc, int delta_msec);
static void ccodoc_tick_kakehi_pouring(ccodoc* ccodoc, int delta_msec);
static void ccodoc_tick_tsutsu_holding(ccodoc* ccodoc, int delta_msec);
static void ccodoc_tick_tsutsu_releasing(ccodoc* ccodoc, int delta_msec);

// tsutsu
static void ccodoc_tsutsu_pour_by(ccodoc_tsutsu* tsutsu, float ratio);
static void ccodoc_tsutsu_set_amount_by(ccodoc_tsutsu* tsutsu, float ratio);

void ccodoc_tick(ccodoc* ccodoc, int delta_msec)
{
    ccodoc_kakehi* kakehi = &ccodoc->kakehi;

    switch (kakehi->state) {
    case ccodoc_kakehi_state_holding:
        ccodoc_tick_kakehi_holding(ccodoc, delta_msec);
        break;
    case ccodoc_kakehi_state_pouring:
        ccodoc_tick_kakehi_pouring(ccodoc, delta_msec);
        break;
    }

    ccodoc_tsutsu* tsutsu = &ccodoc->tsutsu;

    if (kakehi->state == ccodoc_kakehi_state_pouring) {
        ccodoc_tsutsu_pour_by(tsutsu, 0.1);
    }

    switch (tsutsu->state) {
    case ccodoc_tsutsu_state_holding:
        ccodoc_tick_tsutsu_holding(ccodoc, delta_msec);
        break;
    case ccodoc_tsutsu_state_releasing:
        ccodoc_tick_tsutsu_releasing(ccodoc, delta_msec);
        break;
    }
}

static void ccodoc_tick_kakehi_holding(ccodoc* ccodoc, int delta_msec)
{
    ccodoc_kakehi* kakehi = &ccodoc->kakehi;

    ticker_tick(&kakehi->holding_timer.ticker, delta_msec);

    if (!timer_is_timeout(&kakehi->holding_timer)) {
        return;
    }

    kakehi->state = ccodoc_kakehi_state_pouring;
    ticker_reset(&kakehi->pouring_timer.ticker);
}

static void ccodoc_tick_kakehi_pouring(ccodoc* ccodoc, int delta_msec)
{
    ccodoc_kakehi* kakehi = &ccodoc->kakehi;

    ticker_tick(&kakehi->pouring_timer.ticker, delta_msec);

    if (!timer_is_timeout(&kakehi->pouring_timer)) {
        return;
    }

    kakehi->state = ccodoc_kakehi_state_holding;
    ticker_reset(&kakehi->holding_timer.ticker);
}

static void ccodoc_tick_tsutsu_holding(ccodoc* ccodoc, int _delta_msec)
{
    (void)_delta_msec;

    ccodoc_tsutsu* tsutsu = &ccodoc->tsutsu;

    if (ccodoc_tsutsu_holding_ratio(tsutsu) < 1) {
        return;
    }

    tsutsu->state = ccodoc_tsutsu_state_releasing;
    ticker_reset(&tsutsu->releasing_timer.ticker);
}

static void ccodoc_tick_tsutsu_releasing(ccodoc* ccodoc, int delta_msec)
{
    ccodoc_tsutsu* tsutsu = &ccodoc->tsutsu;

    ticker_tick(&tsutsu->releasing_timer.ticker, delta_msec);

    float releasing_ratio = timer_timeout_ratio(&tsutsu->releasing_timer);

    ccodoc_tsutsu_set_amount_by(tsutsu, 1 - releasing_ratio);

    if (releasing_ratio >= 1) {
        tsutsu->state = ccodoc_tsutsu_state_holding;
        tsutsu->amount = 0;
    }
}

static void ccodoc_tsutsu_pour_by(ccodoc_tsutsu* tsutsu, float ratio)
{
    unsigned int delta = tsutsu->capacity * ratio;
    tsutsu->amount = (tsutsu->capacity - tsutsu->amount > delta)
        ? tsutsu->amount + delta
        : tsutsu->capacity;
}

static void ccodoc_tsutsu_set_amount_by(ccodoc_tsutsu* tsutsu, float ratio)
{
    float r = ratio;
    if (ratio < 0) {
        r = 0;
    }
    if (ratio > 1) {
        r = 1;
    }

    tsutsu->amount = tsutsu->capacity * r;
}

float ccodoc_tsutsu_holding_ratio(const ccodoc_tsutsu* tsutsu)
{
    assert(tsutsu->capacity != 0);
    return (float)tsutsu->amount / tsutsu->capacity;
}
