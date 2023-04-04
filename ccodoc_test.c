#include "ccodoc.h"

#include "test.h"

int test_ccodoc(void)
{
    ccodoc_t ccodoc = {
        .kakehi = {
            .release_water_amount = 5,
            .holding_water = {
                .duration = { .msecs = 2500 },
            },
            .releasing_water = {
                .duration = { .msecs = 500 },
            },
        },
        .tsutsu = {
            .water_capacity = 10,
            .releasing_water = {
                .duration = { .msecs = 1500 },
            },
        },
        .hachi = {
            .releasing_water = {
                .duration = { .msecs = 1000 },
            },
        },
    };

    {
        printf("- initial\n");

        EXPECT_TRUE_X(ccodoc.kakehi.state == holding_water);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.kakehi.holding_water) == 0);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.kakehi.releasing_water) == 0);

        EXPECT_TRUE_X(ccodoc.tsutsu.state == holding_water);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.tsutsu.releasing_water) == 0);
        EXPECT_TRUE_X(tsutsu_water_amount_ratio(&ccodoc.tsutsu) == 0);

        EXPECT_TRUE_X(ccodoc.hachi.state == holding_water);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.hachi.releasing_water) == 0);
    }

    {
        printf("- tick (250ms)\n");
        tick_ccodoc(&ccodoc, (duration_t) { .msecs = 250 });

        EXPECT_TRUE_X(ccodoc.kakehi.state == holding_water);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.kakehi.holding_water) == 0.1f);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.kakehi.releasing_water) == 0);

        EXPECT_TRUE_X(ccodoc.tsutsu.state == holding_water);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.tsutsu.releasing_water) == 0);
        EXPECT_TRUE_X(tsutsu_water_amount_ratio(&ccodoc.tsutsu) == 0);

        EXPECT_TRUE_X(ccodoc.hachi.state == holding_water);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.hachi.releasing_water) == 0);
    }

    {
        printf("- tick (1750ms)\n");
        tick_ccodoc(&ccodoc, (duration_t) { .msecs = 1750 });

        EXPECT_TRUE_X(ccodoc.kakehi.state == holding_water);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.kakehi.holding_water) == 0.8f);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.kakehi.releasing_water) == 0);

        EXPECT_TRUE_X(ccodoc.tsutsu.state == holding_water);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.tsutsu.releasing_water) == 0);
        EXPECT_TRUE_X(tsutsu_water_amount_ratio(&ccodoc.tsutsu) == 0);

        EXPECT_TRUE_X(ccodoc.hachi.state == holding_water);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.hachi.releasing_water) == 0);
    }

    {
        printf("- tick (1000ms)\n");
        tick_ccodoc(&ccodoc, (duration_t) { .msecs = 1000 });

        EXPECT_TRUE_X(ccodoc.kakehi.state == releasing_water);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.kakehi.holding_water) == 1);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.kakehi.releasing_water) == 0);

        EXPECT_TRUE_X(ccodoc.tsutsu.state == holding_water);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.tsutsu.releasing_water) == 0);
        EXPECT_TRUE_X(tsutsu_water_amount_ratio(&ccodoc.tsutsu) == 0.5f);

        EXPECT_TRUE_X(ccodoc.hachi.state == holding_water);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.hachi.releasing_water) == 0);
    }

    {
        printf("- tick (250ms)\n");
        tick_ccodoc(&ccodoc, (duration_t) { .msecs = 250 });

        EXPECT_TRUE_X(ccodoc.kakehi.state == releasing_water);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.kakehi.holding_water) == 1);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.kakehi.releasing_water) == 0.5f);

        EXPECT_TRUE_X(ccodoc.tsutsu.state == holding_water);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.tsutsu.releasing_water) == 0);
        EXPECT_TRUE_X(tsutsu_water_amount_ratio(&ccodoc.tsutsu) == 0.5f);

        EXPECT_TRUE_X(ccodoc.hachi.state == holding_water);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.hachi.releasing_water) == 0);
    }

    {
        printf("- tick (250ms)\n");
        tick_ccodoc(&ccodoc, (duration_t) { .msecs = 250 });

        EXPECT_TRUE_X(ccodoc.kakehi.state == holding_water);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.kakehi.holding_water) == 0);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.kakehi.releasing_water) == 1);

        EXPECT_TRUE_X(ccodoc.tsutsu.state == holding_water);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.tsutsu.releasing_water) == 0);
        EXPECT_TRUE_X(tsutsu_water_amount_ratio(&ccodoc.tsutsu) == 0.5f);

        EXPECT_TRUE_X(ccodoc.hachi.state == holding_water);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.hachi.releasing_water) == 0);
    }

    {
        printf("- tick (2500ms)\n");
        tick_ccodoc(&ccodoc, (duration_t) { .msecs = 2500 });

        EXPECT_TRUE_X(ccodoc.kakehi.state == releasing_water);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.kakehi.holding_water) == 1);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.kakehi.releasing_water) == 0);

        EXPECT_TRUE_X(ccodoc.tsutsu.state == releasing_water);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.tsutsu.releasing_water) == 0);
        EXPECT_TRUE_X(tsutsu_water_amount_ratio(&ccodoc.tsutsu) == 0);

        EXPECT_TRUE_X(ccodoc.hachi.state == holding_water);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.hachi.releasing_water) == 1);
    }

    {
        printf("- tick (500ms)\n");
        tick_ccodoc(&ccodoc, (duration_t) { .msecs = 500 });

        EXPECT_TRUE_X(ccodoc.kakehi.state == holding_water);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.kakehi.holding_water) == 0);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.kakehi.releasing_water) == 1);

        EXPECT_TRUE_X(ccodoc.tsutsu.state == releasing_water);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.tsutsu.releasing_water) == 1.0f / 3);
        EXPECT_TRUE_X(tsutsu_water_amount_ratio(&ccodoc.tsutsu) == 0);

        EXPECT_TRUE_X(ccodoc.hachi.state == holding_water);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.hachi.releasing_water) == 1);
    }

    {
        printf("- tick (1000ms)\n");
        tick_ccodoc(&ccodoc, (duration_t) { .msecs = 1000 });

        EXPECT_TRUE_X(ccodoc.kakehi.state == holding_water);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.kakehi.holding_water) == 0.4f);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.kakehi.releasing_water) == 1);

        EXPECT_TRUE_X(ccodoc.tsutsu.state == holding_water);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.tsutsu.releasing_water) == 1);
        EXPECT_TRUE_X(tsutsu_water_amount_ratio(&ccodoc.tsutsu) == 0);

        EXPECT_TRUE_X(ccodoc.hachi.state == holding_water);
        EXPECT_TRUE_X(action_progress_ratio(&ccodoc.hachi.releasing_water) == 1);
    }

    return EXIT_SUCCESS;
}
