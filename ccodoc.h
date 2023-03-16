#include <assert.h>
#include <curses.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    unsigned long msec;
} duration;

typedef struct {
    duration elapsed;
} ticker;

typedef struct {
    duration duration;
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

typedef enum {
    ccodoc_hachi_state_holding,
    ccodoc_hachi_state_releasing,
} ccodoc_hachi_state;

// hachi（手水鉢）
typedef struct {
    ccodoc_hachi_state state;
    timer releasing_timer;
} ccodoc_hachi;

// ccodoc（鹿威し）
typedef struct {
    ccodoc_kakehi kakehi;
    ccodoc_tsutsu tsutsu;
    ccodoc_hachi hachi;
} ccodoc;

typedef struct {
    WINDOW* window;
} ccodoc_renderer;

// ccodoc.c
extern void ccodoc_tick(ccodoc* ccodoc, const duration delta);
extern float ccodoc_tsutsu_holding_ratio(const ccodoc_tsutsu* tsutsu);

// renderer.c
extern void ccodoc_init_renderer(ccodoc_renderer* renderer);
extern void ccodoc_deinit_renderer(ccodoc_renderer* renderer);
extern void ccodoc_render(ccodoc_renderer* renderer, const ccodoc_context* ctx, const ccodoc* ccodoc);

// string.c
extern bool str_equals_to(char* str, char* other);

// time.c
extern void ticker_tick(ticker* ticker, const duration delta);
extern void ticker_reset(ticker* ticker);
extern bool timer_is_timeout(const timer* timer);
extern float timer_timeout_ratio(const timer* timer);
extern void sleep_for(const duration duration);
