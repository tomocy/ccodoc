#include <assert.h>
#include <stdlib.h>

typedef enum {
    ccodoc_state_holding,
    ccodoc_state_releasing,
} ccodoc_state;

typedef struct ccodoc {
    ccodoc_state state;
    unsigned int water_amount;
    unsigned int water_capacity;
} ccodoc;

// ccodoc.c
extern const size_t ccodoc_art_height;
extern const char* ccodoc_art_jo[];
extern const char* ccodoc_art_ha[];
extern const char* ccodoc_art_kyu[];
extern void ccodoc_pour_water(ccodoc* ccodoc, int water_amount);
extern void ccodoc_release_water(ccodoc* ccodoc, int water_amount);
extern float ccodoc_water_ratio(const ccodoc* ccodoc);

// time.c
extern void sleep_msec(int msec);
