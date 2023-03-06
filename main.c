#include <stdio.h>
#include <stdlib.h>

#include "ccodoc.h"

int main()
{
    unsigned int elapsed_sec = 0;

    ccodoc ccodoc = {
        .water_amount = 0,
        .water_capacity = 10,
    };

    for (unsigned int i = 0;; i++) {
        system("clear");

        int water_poured = i % 4;

        for (int j = 0; j < 3; j++) {
            char* water = (j == water_poured) ? "━" : "═";
            printf("%s", water);
        }
        printf("\n");

        if (water_poured) {
            ccodoc_pour_water(&ccodoc, 1);
        }

        const char** ccodoc_art = NULL;

        float water_ratio = ccodoc_water_ratio(&ccodoc);
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

        ccodoc_release_water(&ccodoc, 1);

        sleep_msec(500);

        if (i % 2 != 0) {
            continue;
        }

        elapsed_sec++;
    }

    return 0;
}
