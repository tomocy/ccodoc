#include "ccodoc.h"
#include <stdio.h>

static const int water_spout_len = 3;

void ccodoc_render(
    const ccodoc* ccodoc,
    float water_spout_ratio
)
{
    system("clear");

    int water_spout_index = 0;
    if (0.3 <= water_spout_ratio && water_spout_ratio < 0.6) {
        water_spout_index = 1;
    }
    if (0.6 <= water_spout_ratio && water_spout_ratio < 0.9) {
        water_spout_index = 2;
    }
    if (water_spout_ratio >= 0.9) {
        water_spout_index = 3;
    }

    for (int j = 0; j < water_spout_len; j++) {
        char* water = (j == water_spout_index) ? "━" : "═";
        printf("%s", water);
    }
    printf("\n");

    const char** ccodoc_art = NULL;

    float water_ratio = ccodoc_water_ratio(ccodoc);
    if (water_ratio < 0.8) {
        ccodoc_art = ccodoc_art_jo;
    } else if (water_ratio < 1) {
        ccodoc_art = ccodoc_art_ha;
    } else {
        ccodoc_art = ccodoc_art_kyu;
    }

    assert(ccodoc_art != NULL);

    for (size_t h = 0; h < ccodoc_art_height; h++) {
        printf("   %s\n", ccodoc_art[h]);
    }

    printf("▭▭▭▭━━━━━━▨▨▨▨\n");
}
