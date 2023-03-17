#include "ccodoc.h"
#include <stdio.h>

static void configure_with_args(ccodoc_context* ctx, int argc, char** argv);
static int run(const ccodoc_context* ctx, ccodoc* ccodoc);

int main(int argc, char** argv)
{
    ccodoc_context ctx = {
        .fps = 24,
        .debug = false,
    };

    configure_with_args(&ctx, argc, argv);

    ccodoc ccodoc = {
        .kakehi = {
            .release_water_ratio = 0.1f,
            .holding_water_timer = {
                .duration = { .msec = 2500 },
            },
            .releasing_water_timer = {
                .duration = { .msec = 500 },
            },
        },
        .tsutsu = {
            .water_capacity = 10,
            .releasing_water_timer = {
                .duration = { .msec = 1800 },
            },
        },
        .hachi = {
            .releasing_water_timer = {
                .duration = { .msec = 1000 },
            },
        },
    };

    return run(&ctx, &ccodoc);
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
    }
}

static int run(const ccodoc_context* ctx, ccodoc* ccodoc)
{
    assert(ctx->fps != 0);

    const duration delta = {
        .msec = 1000 / ctx->fps,
    };

    ccodoc_renderer renderer = { 0 };

    ccodoc_init_renderer(&renderer);

    while (1) {
        ccodoc_tick(ccodoc, delta);

        ccodoc_render(&renderer, ctx, ccodoc);

        sleep_for(delta);
    }

    ccodoc_deinit_renderer(&renderer);

    return 0;
}
