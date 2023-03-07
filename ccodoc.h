#include <assert.h>
#include <stdlib.h>

typedef struct {
    long sec;
    int msec;
} ticker;

typedef enum {
    ccodoc_kakehi_state_holding,
    ccodoc_kakehi_state_pouring,
} ccodoc_kakehi_state;

// kakehi（筧）
typedef struct {
    ccodoc_kakehi_state state;

    int holding_period_msec;
    ticker holding_ticker;

    int pouring_period_msec;
    ticker pouring_ticker;
} ccodoc_kakehi;

typedef enum {
    ccodoc_tsutsu_state_holding,
    ccodoc_tsutsu_state_releasing,
} ccodoc_tsutsu_state;

// tsutsu（筒）
typedef struct {
    ccodoc_tsutsu_state state;
    unsigned int water_amount;
    unsigned int water_capacity;
} ccodoc_tsutsu;

// ccodoc (鹿威し)
typedef struct {
    ccodoc_kakehi kakehi;
    ccodoc_tsutsu tsutsu;
} ccodoc;

// ccodoc.c
extern void ccodoc_tick(ccodoc* ccodoc, int delta_msec);
extern float ccodoc_tsutsu_holding_water_ratio(const ccodoc_tsutsu* tsutsu);

// time.c
extern void ticker_tick(ticker* clock, int delta_msec);
extern void ticker_reset(ticker* clock);
extern int ticker_elapsed_msec(const ticker* clock);
extern void sleep_msec(int msec);

// view.c
extern void ccodoc_render(const ccodoc* ccodoc);
