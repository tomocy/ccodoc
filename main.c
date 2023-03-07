#include <stdio.h>
#include <stdlib.h>

#include "ccodoc.h"

int main(void)
{
    const int delta_msec = 500;

    ccodoc ccodoc = {
        .kakehi = {
            .holding_timer = {
                .period_msec = 1500,
            },
            .pouring_timer = {
                .period_msec = 500,
            },
        },
        .tsutsu = {
            .capacity = 10,
            .releasing_timer = {
                .period_msec = 2000,
            },
        },
    };

    while (1) {
        ccodoc_tick(&ccodoc, delta_msec);

        ccodoc_render(&ccodoc);

        sleep_msec(delta_msec);
    }

    return 0;
}
