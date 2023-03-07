#include "ccodoc.h"

static void config_with_args(ccodoc_context* ctx, int argc, char** argv);
static int run(const ccodoc_context* ctx, ccodoc* ccodoc);

int main(int argc, char** argv)
{
    ccodoc_context ctx = {
        .debug = false,
    };

    config_with_args(&ctx, argc, argv);

    ccodoc ccodoc = {
        .kakehi = {
            .pour_ratio = 0.1,
            .holding_timer = {
                .period_msec = 2500,
            },
            .pouring_timer = {
                .period_msec = 500,
            },
        },
        .tsutsu = {
            .capacity = 10,
            .releasing_timer = {
                .period_msec = 1800,
            },
        },
    };

    return run(&ctx, &ccodoc);
}

static void config_with_args(ccodoc_context* ctx, int argc, char** argv)
{
    for (int i = 1; i < argc; i++) {
        char* arg = argv[i];

        if (str_equal_to(arg, "-debug")) {
            ctx->debug = true;
        }
    }
}

static int run(const ccodoc_context* ctx, ccodoc* ccodoc)
{
    static const int delta_msec = 100;

    while (1) {
        ccodoc_tick(ccodoc, delta_msec);

        ccodoc_render(ctx, ccodoc);

        sleep_msec(delta_msec);
    }

    return 0;
}
