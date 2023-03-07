#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    long sec;
    int msec;
} ticker;

typedef struct {
    int period_msec;
    ticker ticker;
} timer;

typedef struct {
    bool debug;
} ccodoc_context;

typedef enum {
    ccodoc_kakehi_state_holding,
    ccodoc_kakehi_state_pouring,
} ccodoc_kakehi_state;

// kakehi（筧）
typedef struct {
    ccodoc_kakehi_state state;
    float pour_ratio;
    timer holding_timer;
    timer pouring_timer;
} ccodoc_kakehi;

typedef enum {
    ccodoc_tsutsu_state_holding,
    ccodoc_tsutsu_state_releasing,
} ccodoc_tsutsu_state;

// tsutsu（筒）
typedef struct {
    ccodoc_tsutsu_state state;
    unsigned int amount;
    unsigned int capacity;
    timer releasing_timer;
} ccodoc_tsutsu;

// ccodoc (鹿威し)
typedef struct {
    ccodoc_kakehi kakehi;
    ccodoc_tsutsu tsutsu;
} ccodoc;

// ccodoc.c
extern void ccodoc_tick(ccodoc* ccodoc, int delta_msec);
extern float ccodoc_tsutsu_holding_ratio(const ccodoc_tsutsu* tsutsu);

// view.c
extern void ccodoc_render(const ccodoc_context* ctx, const ccodoc* ccodoc);

// string.c
extern bool str_equal_to(char* str, char* other);

// time.c
extern void ticker_tick(ticker* ticker, int delta_msec);
extern void ticker_reset(ticker* ticker);
extern int ticker_elapsed_msec(const ticker* ticker);
extern bool timer_is_timeout(const timer* timer);
extern float timer_timeout_ratio(const timer* timer);
extern void sleep_msec(int msec);
