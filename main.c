#include "ccodoc.h"
#include <stdio.h>

static void configure_with_args(ccodoc_context* ctx, int argc, char** argv);
static int run(const ccodoc_context* ctx, timer* timer, ccodoc* ccodoc);
static int test(void);

int main(int argc, char** argv)
{
    ccodoc_context ctx = {
        .fps = 24,
        .debug = false,
        .duration = duration_from_moment((moment) {
            .mins = 30,
        }),
    };

    configure_with_args(&ctx, argc, argv);

    timer timer = { .duration = ctx.duration };

    ccodoc ccodoc = {
        .kakehi = {
            .release_water_ratio = 0.1f,
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
                .duration = { .msecs = 1800 },
            },
        },
        .hachi = {
            .releasing_water_timer = {
                .duration = { .msecs = 1000 },
            },
        },
    };

    if (ctx.test) {
        return test();
    }

    return run(&ctx, &timer, &ccodoc);
}

static void configure_with_args(ccodoc_context* ctx, int argc, char** argv)
{
    for (int i = 1; i < argc; i++) {
        char* arg = argv[i];

        if (str_equals_to(arg, "--fps")) {
            const unsigned long fps = strtoul(argv[i + 1], NULL, 10);
            if (fps != 0) {
                ctx->fps = fps;
            }
        }

        if (str_equals_to(arg, "--debug")) {
            ctx->debug = true;
        }

        if (str_equals_to(arg, "--test")) {
            ctx->test = true;
        }

        if (str_equals_to(arg, "--duration")) {
            moment m = { 0 };
            // NOLINTNEXTLINE(cert-err34-c)
            (void)sscanf(argv[i + 1], "%d:%d", &m.hours, &m.mins);

            const duration d = duration_from_moment(m);
            if (d.msecs != 0) {
                ctx->duration = d;
            }
        }
    }
}

static int run(const ccodoc_context* ctx, timer* timer, ccodoc* ccodoc)
{
    assert(ctx->fps != 0);

    const duration delta = {
        .msecs = 1000 / ctx->fps,
    };

    ccodoc_renderer renderer = { 0 };

    ccodoc_init_renderer(&renderer);

    while (1) {
        timer_tick(timer, delta);
        ccodoc_tick(ccodoc, delta);

        ccodoc_render(&renderer, ctx, timer, ccodoc);

        sleep_for(delta);
    }

    ccodoc_deinit_renderer(&renderer);

    return 0;
}

static int test(void)
{
    test_ccodoc();
    printf("\n");

    test_str();
    printf("\n");

    test_time();
    printf("\n");

    printf("OK\n");

    return 0;
}
