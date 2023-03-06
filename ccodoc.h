#include <assert.h>
#include <stdlib.h>

// ccodoc.c
typedef enum {
    ccodoc_state_holding,
    ccodoc_state_releasing,
} ccodoc_state;

typedef struct {
    ccodoc_state state;
    unsigned int water_amount;
    unsigned int water_capacity;
} ccodoc;

extern const size_t ccodoc_art_height;
extern const char* ccodoc_art_jo[];
extern const char* ccodoc_art_ha[];
extern const char* ccodoc_art_kyu[];
extern void ccodoc_pour_water(ccodoc* ccodoc, int water_amount);
extern void ccodoc_release_water(ccodoc* ccodoc, int water_amount);
extern float ccodoc_water_ratio(const ccodoc* ccodoc);

// time.c
typedef struct {
    long sec;
    int msec;
} ticker;

extern void ticker_tick(ticker* clock, int delta_msec);
extern void ticker_reset(ticker* clock);
extern int ticker_elapsed_msec(const ticker* clock);
extern void sleep_msec(int msec);

// view.c
extern void ccodoc_render(const ccodoc* ccodoc, float water_spout_ratio);
