#include "ccodoc.h"

static void ccodoc_tick_kakehi_holding(ccodoc* ccodoc, duration delta);
static void ccodoc_tick_kakehi_pouring(ccodoc* ccodoc, duration delta);
static void ccodoc_tick_tsutsu_holding(ccodoc* ccodoc, duration delta);
static void ccodoc_tick_tsutsu_releasing(ccodoc* ccodoc, duration delta);
static void ccodoc_tsutsu_pour_by(ccodoc_tsutsu* tsutsu, float ratio);
static void ccodoc_tsutsu_set_amount_by(ccodoc_tsutsu* tsutsu, float ratio);

void ccodoc_tick(ccodoc* ccodoc, const duration delta)
{
    switch (ccodoc->kakehi.state) {
    case ccodoc_kakehi_state_holding:
        ccodoc_tick_kakehi_holding(ccodoc, delta);
        break;
    case ccodoc_kakehi_state_pouring:
        ccodoc_tick_kakehi_pouring(ccodoc, delta);
        break;
    }

    switch (ccodoc->tsutsu.state) {
    case ccodoc_tsutsu_state_holding:
        ccodoc_tick_tsutsu_holding(ccodoc, delta);
        break;
    case ccodoc_tsutsu_state_releasing:
        ccodoc_tick_tsutsu_releasing(ccodoc, delta);
        break;
    }
}

static void ccodoc_tick_kakehi_holding(ccodoc* ccodoc, const duration delta)
{
    ccodoc_kakehi* kakehi = &ccodoc->kakehi;

    ticker_tick(&kakehi->holding_timer.ticker, delta);

    if (!timer_is_timeout(&kakehi->holding_timer)) {
        return;
    }

    kakehi->state = ccodoc_kakehi_state_pouring;
    ticker_reset(&kakehi->pouring_timer.ticker);

    ccodoc_tsutsu_pour_by(&ccodoc->tsutsu, kakehi->pour_ratio);
}

static void ccodoc_tick_kakehi_pouring(ccodoc* ccodoc, const duration delta)
{
    ccodoc_kakehi* kakehi = &ccodoc->kakehi;

    ticker_tick(&kakehi->pouring_timer.ticker, delta);

    if (!timer_is_timeout(&kakehi->pouring_timer)) {
        return;
    }

    kakehi->state = ccodoc_kakehi_state_holding;
    ticker_reset(&kakehi->holding_timer.ticker);
}

static void ccodoc_tick_tsutsu_holding(ccodoc* ccodoc, const duration _delta)
{
    (void)_delta;

    ccodoc_tsutsu* tsutsu = &ccodoc->tsutsu;

    if (ccodoc_tsutsu_holding_ratio(tsutsu) < 1) {
        return;
    }

    tsutsu->state = ccodoc_tsutsu_state_releasing;
    ticker_reset(&tsutsu->releasing_timer.ticker);
}

static void ccodoc_tick_tsutsu_releasing(ccodoc* ccodoc, const duration delta)
{
    ccodoc_tsutsu* tsutsu = &ccodoc->tsutsu;

    ticker_tick(&tsutsu->releasing_timer.ticker, delta);

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
    return (double)tsutsu->amount / tsutsu->capacity;
}
