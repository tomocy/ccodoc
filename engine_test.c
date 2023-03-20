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

        EXPECT(ccodoc.kakehi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.holding_water_timer), 0);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.releasing_water_timer), 0);

        EXPECT(ccodoc.tsutsu.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT(ccodoc_tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0);

        EXPECT(ccodoc.hachi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.hachi.releasing_water_timer), 0);
    }

    {
        printf("- tick (250ms)\n");
        ccodoc_tick(&ccodoc, (duration) { .msecs = 250 });

        EXPECT(ccodoc.kakehi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.holding_water_timer), 0.1f);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.releasing_water_timer), 0);

        EXPECT(ccodoc.tsutsu.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT(ccodoc_tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0);

        EXPECT(ccodoc.hachi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.hachi.releasing_water_timer), 0);
    }

    {
        printf("- tick (1750ms)\n");
        ccodoc_tick(&ccodoc, (duration) { .msecs = 1750 });

        EXPECT(ccodoc.kakehi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.holding_water_timer), 0.8f);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.releasing_water_timer), 0);

        EXPECT(ccodoc.tsutsu.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT(ccodoc_tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0);

        EXPECT(ccodoc.hachi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.hachi.releasing_water_timer), 0);
    }

    {
        printf("- tick (1000ms)\n");
        ccodoc_tick(&ccodoc, (duration) { .msecs = 1000 });

        EXPECT(ccodoc.kakehi.state, ccodoc_releasing_water);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.holding_water_timer), 1.2f);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.releasing_water_timer), 0);

        EXPECT(ccodoc.tsutsu.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT(ccodoc_tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0.5f);

        EXPECT(ccodoc.hachi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.hachi.releasing_water_timer), 0);
    }

    {
        printf("- tick (250ms)\n");
        ccodoc_tick(&ccodoc, (duration) { .msecs = 250 });

        EXPECT(ccodoc.kakehi.state, ccodoc_releasing_water);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.holding_water_timer), 1.2f);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.releasing_water_timer), 0.5f);

        EXPECT(ccodoc.tsutsu.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT(ccodoc_tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0.5f);

        EXPECT(ccodoc.hachi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.hachi.releasing_water_timer), 0);
    }

    {
        printf("- tick (250ms)\n");
        ccodoc_tick(&ccodoc, (duration) { .msecs = 250 });

        EXPECT(ccodoc.kakehi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.holding_water_timer), 0);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.releasing_water_timer), 1);

        EXPECT(ccodoc.tsutsu.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT(ccodoc_tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0.5f);

        EXPECT(ccodoc.hachi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.hachi.releasing_water_timer), 0);
    }

    {
        printf("- tick (2500ms)\n");
        ccodoc_tick(&ccodoc, (duration) { .msecs = 2500 });

        EXPECT(ccodoc.kakehi.state, ccodoc_releasing_water);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.holding_water_timer), 1);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.releasing_water_timer), 0);

        EXPECT(ccodoc.tsutsu.state, ccodoc_releasing_water);
        EXPECT(timer_timeout_ratio(&ccodoc.tsutsu.releasing_water_timer), 0);
        EXPECT(ccodoc_tsutsu_water_amount_ratio(&ccodoc.tsutsu), 1);

        EXPECT(ccodoc.hachi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.hachi.releasing_water_timer), 2.5);
    }

    {
        printf("- tick (500ms)\n");
        ccodoc_tick(&ccodoc, (duration) { .msecs = 500 });

        EXPECT(ccodoc.kakehi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.holding_water_timer), 0);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.releasing_water_timer), 1);

        EXPECT(ccodoc.tsutsu.state, ccodoc_releasing_water);
        EXPECT(timer_timeout_ratio(&ccodoc.tsutsu.releasing_water_timer), 1.0f / 3);
        EXPECT(ccodoc_tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0.6f);

        EXPECT(ccodoc.hachi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.hachi.releasing_water_timer), 2.5);
    }

    {
        printf("- tick (1000ms)\n");
        ccodoc_tick(&ccodoc, (duration) { .msecs = 1000 });

        EXPECT(ccodoc.kakehi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.holding_water_timer), 0.4f);
        EXPECT(timer_timeout_ratio(&ccodoc.kakehi.releasing_water_timer), 1);

        EXPECT(ccodoc.tsutsu.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.tsutsu.releasing_water_timer), 1);
        EXPECT(ccodoc_tsutsu_water_amount_ratio(&ccodoc.tsutsu), 0);

        EXPECT(ccodoc.hachi.state, ccodoc_holding_water);
        EXPECT(timer_timeout_ratio(&ccodoc.hachi.releasing_water_timer), 2.5);
    }

    return EXIT_SUCCESS;
}
