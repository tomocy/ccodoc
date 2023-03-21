#include "ccodoc.h"
#include "ccodoc_test.h"

int test_engine(void)
{
    ccodoc ccodoc = {
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

        EXPECT(ccodoc.kakehi.state, holding_water);
        EXPECT(elapsed_time_ratio(&ccodoc.kakehi.holding_water_timer), 0);
        EXPECT(elapsed_time_ratio(&ccodoc.kakehi.releasing_water_timer), 0);

        EXPECT(ccodoc.tsutsu.state, holding_water);
        EXPECT(elapsed_time_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT(tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0);

        EXPECT(ccodoc.hachi.state, holding_water);
        EXPECT(elapsed_time_ratio(&ccodoc.hachi.releasing_water_timer), 0);
    }

    {
        printf("- tick (250ms)\n");
        tick_ccodoc(&ccodoc, (duration) { .msecs = 250 });

        EXPECT(ccodoc.kakehi.state, holding_water);
        EXPECT(elapsed_time_ratio(&ccodoc.kakehi.holding_water_timer), 0.1f);
        EXPECT(elapsed_time_ratio(&ccodoc.kakehi.releasing_water_timer), 0);

        EXPECT(ccodoc.tsutsu.state, holding_water);
        EXPECT(elapsed_time_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT(tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0);

        EXPECT(ccodoc.hachi.state, holding_water);
        EXPECT(elapsed_time_ratio(&ccodoc.hachi.releasing_water_timer), 0);
    }

    {
        printf("- tick (1750ms)\n");
        tick_ccodoc(&ccodoc, (duration) { .msecs = 1750 });

        EXPECT(ccodoc.kakehi.state, holding_water);
        EXPECT(elapsed_time_ratio(&ccodoc.kakehi.holding_water_timer), 0.8f);
        EXPECT(elapsed_time_ratio(&ccodoc.kakehi.releasing_water_timer), 0);

        EXPECT(ccodoc.tsutsu.state, holding_water);
        EXPECT(elapsed_time_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT(tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0);

        EXPECT(ccodoc.hachi.state, holding_water);
        EXPECT(elapsed_time_ratio(&ccodoc.hachi.releasing_water_timer), 0);
    }

    {
        printf("- tick (1000ms)\n");
        tick_ccodoc(&ccodoc, (duration) { .msecs = 1000 });

        EXPECT(ccodoc.kakehi.state, releasing_water);
        EXPECT(elapsed_time_ratio(&ccodoc.kakehi.holding_water_timer), 1.2f);
        EXPECT(elapsed_time_ratio(&ccodoc.kakehi.releasing_water_timer), 0);

        EXPECT(ccodoc.tsutsu.state, holding_water);
        EXPECT(elapsed_time_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT(tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0.5f);

        EXPECT(ccodoc.hachi.state, holding_water);
        EXPECT(elapsed_time_ratio(&ccodoc.hachi.releasing_water_timer), 0);
    }

    {
        printf("- tick (250ms)\n");
        tick_ccodoc(&ccodoc, (duration) { .msecs = 250 });

        EXPECT(ccodoc.kakehi.state, releasing_water);
        EXPECT(elapsed_time_ratio(&ccodoc.kakehi.holding_water_timer), 1.2f);
        EXPECT(elapsed_time_ratio(&ccodoc.kakehi.releasing_water_timer), 0.5f);

        EXPECT(ccodoc.tsutsu.state, holding_water);
        EXPECT(elapsed_time_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT(tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0.5f);

        EXPECT(ccodoc.hachi.state, holding_water);
        EXPECT(elapsed_time_ratio(&ccodoc.hachi.releasing_water_timer), 0);
    }

    {
        printf("- tick (250ms)\n");
        tick_ccodoc(&ccodoc, (duration) { .msecs = 250 });

        EXPECT(ccodoc.kakehi.state, holding_water);
        EXPECT(elapsed_time_ratio(&ccodoc.kakehi.holding_water_timer), 0);
        EXPECT(elapsed_time_ratio(&ccodoc.kakehi.releasing_water_timer), 1);

        EXPECT(ccodoc.tsutsu.state, holding_water);
        EXPECT(elapsed_time_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT(tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0.5f);

        EXPECT(ccodoc.hachi.state, holding_water);
        EXPECT(elapsed_time_ratio(&ccodoc.hachi.releasing_water_timer), 0);
    }

    {
        printf("- tick (2500ms)\n");
        tick_ccodoc(&ccodoc, (duration) { .msecs = 2500 });

        EXPECT(ccodoc.kakehi.state, releasing_water);
        EXPECT(elapsed_time_ratio(&ccodoc.kakehi.holding_water_timer), 1);
        EXPECT(elapsed_time_ratio(&ccodoc.kakehi.releasing_water_timer), 0);

        EXPECT(ccodoc.tsutsu.state, releasing_water);
        EXPECT(elapsed_time_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT(tsutsu_water_amount_ratio(&ccodoc.tsutsu), 1);

        EXPECT(ccodoc.hachi.state, holding_water);
        EXPECT(elapsed_time_ratio(&ccodoc.hachi.releasing_water_timer), 2.5);
    }

    {
        printf("- tick (500ms)\n");
        tick_ccodoc(&ccodoc, (duration) { .msecs = 500 });

        EXPECT(ccodoc.kakehi.state, holding_water);
        EXPECT(elapsed_time_ratio(&ccodoc.kakehi.holding_water_timer), 0);
        EXPECT(elapsed_time_ratio(&ccodoc.kakehi.releasing_water_timer), 1);

        EXPECT(ccodoc.tsutsu.state, releasing_water);
        EXPECT(elapsed_time_ratio(&ccodoc.tsutsu.releasing_water_timer), 1.0f / 3);
        EXPECT(tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0.6f);

        EXPECT(ccodoc.hachi.state, holding_water);
        EXPECT(elapsed_time_ratio(&ccodoc.hachi.releasing_water_timer), 2.5);
    }

    {
        printf("- tick (1000ms)\n");
        tick_ccodoc(&ccodoc, (duration) { .msecs = 1000 });

        EXPECT(ccodoc.kakehi.state, holding_water);
        EXPECT(elapsed_time_ratio(&ccodoc.kakehi.holding_water_timer), 0.4f);
        EXPECT(elapsed_time_ratio(&ccodoc.kakehi.releasing_water_timer), 1);

        EXPECT(ccodoc.tsutsu.state, holding_water);
        EXPECT(elapsed_time_ratio(&ccodoc.tsutsu.releasing_water_timer), 1);
        EXPECT(tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0);

        EXPECT(ccodoc.hachi.state, holding_water);
        EXPECT(elapsed_time_ratio(&ccodoc.hachi.releasing_water_timer), 2.5);
    }

    return EXIT_SUCCESS;
}
