#include "ccodoc.h"

#include "ccodoc_test.h"

int test_ccodoc(void)
{
    ccodoc_t ccodoc = {
        .kakehi = {
            .release_water_ratio = 0.5f,
            .holding_water_timer = {
                .duration = { .msecs = 2500 },
            },
            .releasing_water_timer = {
                .duration = { .msecs = 500 },
            },
        },
        .tsutsu = {
            .water_capacity = 10,
            .releasing_water_timer = {
                .duration = { .msecs = 1500 },
            },
        },
        .hachi = {
            .releasing_water_timer = {
                .duration = { .msecs = 1000 },
            },
        },
    };

    {
        printf("- initial\n");

        EXPECT_EQUAL(ccodoc.kakehi.state, holding_water);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.kakehi.holding_water_timer), 0);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.kakehi.releasing_water_timer), 0);

        EXPECT_EQUAL(ccodoc.tsutsu.state, holding_water);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT_EQUAL(tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0);

        EXPECT_EQUAL(ccodoc.hachi.state, holding_water);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.hachi.releasing_water_timer), 0);
    }

    {
        printf("- tick (250ms)\n");
        tick_ccodoc(&ccodoc, (duration_t) { .msecs = 250 });

        EXPECT_EQUAL(ccodoc.kakehi.state, holding_water);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.kakehi.holding_water_timer), 0.1f);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.kakehi.releasing_water_timer), 0);

        EXPECT_EQUAL(ccodoc.tsutsu.state, holding_water);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT_EQUAL(tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0);

        EXPECT_EQUAL(ccodoc.hachi.state, holding_water);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.hachi.releasing_water_timer), 0);
    }

    {
        printf("- tick (1750ms)\n");
        tick_ccodoc(&ccodoc, (duration_t) { .msecs = 1750 });

        EXPECT_EQUAL(ccodoc.kakehi.state, holding_water);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.kakehi.holding_water_timer), 0.8f);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.kakehi.releasing_water_timer), 0);

        EXPECT_EQUAL(ccodoc.tsutsu.state, holding_water);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT_EQUAL(tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0);

        EXPECT_EQUAL(ccodoc.hachi.state, holding_water);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.hachi.releasing_water_timer), 0);
    }

    {
        printf("- tick (1000ms)\n");
        tick_ccodoc(&ccodoc, (duration_t) { .msecs = 1000 });

        EXPECT_EQUAL(ccodoc.kakehi.state, releasing_water);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.kakehi.holding_water_timer), 1);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.kakehi.releasing_water_timer), 0);

        EXPECT_EQUAL(ccodoc.tsutsu.state, holding_water);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT_EQUAL(tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0.5f);

        EXPECT_EQUAL(ccodoc.hachi.state, holding_water);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.hachi.releasing_water_timer), 0);
    }

    {
        printf("- tick (250ms)\n");
        tick_ccodoc(&ccodoc, (duration_t) { .msecs = 250 });

        EXPECT_EQUAL(ccodoc.kakehi.state, releasing_water);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.kakehi.holding_water_timer), 1);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.kakehi.releasing_water_timer), 0.5f);

        EXPECT_EQUAL(ccodoc.tsutsu.state, holding_water);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT_EQUAL(tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0.5f);

        EXPECT_EQUAL(ccodoc.hachi.state, holding_water);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.hachi.releasing_water_timer), 0);
    }

    {
        printf("- tick (250ms)\n");
        tick_ccodoc(&ccodoc, (duration_t) { .msecs = 250 });

        EXPECT_EQUAL(ccodoc.kakehi.state, holding_water);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.kakehi.holding_water_timer), 0);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.kakehi.releasing_water_timer), 1);

        EXPECT_EQUAL(ccodoc.tsutsu.state, holding_water);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT_EQUAL(tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0.5f);

        EXPECT_EQUAL(ccodoc.hachi.state, holding_water);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.hachi.releasing_water_timer), 0);
    }

    {
        printf("- tick (2500ms)\n");
        tick_ccodoc(&ccodoc, (duration_t) { .msecs = 2500 });

        EXPECT_EQUAL(ccodoc.kakehi.state, releasing_water);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.kakehi.holding_water_timer), 1);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.kakehi.releasing_water_timer), 0);

        EXPECT_EQUAL(ccodoc.tsutsu.state, releasing_water);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT_EQUAL(tsutsu_water_amount_ratio(&ccodoc.tsutsu), 1);

        EXPECT_EQUAL(ccodoc.hachi.state, holding_water);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.hachi.releasing_water_timer), 1);
    }

    {
        printf("- tick (500ms)\n");
        tick_ccodoc(&ccodoc, (duration_t) { .msecs = 500 });

        EXPECT_EQUAL(ccodoc.kakehi.state, holding_water);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.kakehi.holding_water_timer), 0);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.kakehi.releasing_water_timer), 1);

        EXPECT_EQUAL(ccodoc.tsutsu.state, releasing_water);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.tsutsu.releasing_water_timer), 1.0f / 3);
        EXPECT_EQUAL(tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0.6f);

        EXPECT_EQUAL(ccodoc.hachi.state, holding_water);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.hachi.releasing_water_timer), 1);
    }

    {
        printf("- tick (1000ms)\n");
        tick_ccodoc(&ccodoc, (duration_t) { .msecs = 1000 });

        EXPECT_EQUAL(ccodoc.kakehi.state, holding_water);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.kakehi.holding_water_timer), 0.4f);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.kakehi.releasing_water_timer), 1);

        EXPECT_EQUAL(ccodoc.tsutsu.state, holding_water);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.tsutsu.releasing_water_timer), 1);
        EXPECT_EQUAL(tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0);

        EXPECT_EQUAL(ccodoc.hachi.state, holding_water);
        EXPECT_EQUAL(elapsed_time_ratio(&ccodoc.hachi.releasing_water_timer), 1);
    }

    return EXIT_SUCCESS;
}
