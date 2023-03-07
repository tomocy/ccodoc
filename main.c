#include <stdio.h>
#include <stdlib.h>

#include "ccodoc.h"

int main()
{
    const int delta_msec = 500;

    ccodoc ccodoc = {
        .kakehi = {
            .holding_period_msec = 1500,
            .pouring_period_msec = 500,
        },
        .tsutsu = {
            .water_capacity = 10,
        },
    };

    while (1) {
        ccodoc_tick(&ccodoc, delta_msec);

        ccodoc_render(&ccodoc);

        sleep_msec(delta_msec);
    }

    return 0;
}
