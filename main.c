#include <stdio.h>
#include <stdlib.h>

#include "ccodoc.h"

int main()
{
    const int delta_msec = 500;

    ccodoc ccodoc = {
        .water_amount = 0,
        .water_capacity = 10,
    };

    const int pour_water_period_msec = 2000;
    ticker pour_water_ticker = {
        .sec = 0,
        .msec = 0,
    };

    while (1) {
        float water_spout_ratio = (float)ticker_elapsed_msec(&pour_water_ticker)
            / pour_water_period_msec;

        ccodoc_render(&ccodoc, water_spout_ratio);

        ccodoc_release_water(&ccodoc, 1);

        if (water_spout_ratio >= 1) {
            ccodoc_pour_water(&ccodoc, 1);
            ticker_reset(&pour_water_ticker);
        }

        sleep_msec(delta_msec);
        ticker_tick(&pour_water_ticker, delta_msec);
    }

    return 0;
}
