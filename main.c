#include "ccodoc.h"
#include <stdio.h>

static const char* configure_with_args(ccodoc_context* ctx, int argc, const char** argv);
static int help(void);
static int test(void);
static int run(const ccodoc_context* ctx, timer* timer, ccodoc* ccodoc);

int main(int argc, const char** argv)
{
    ccodoc_context ctx = {
        .debug = false,
        .duration = duration_from_moment((moment) {
            .mins = 30,
        }),
    };

    {
        const char* err = configure_with_args(&ctx, argc, argv);
        if (err != NULL) {
            help();
            printf("\n");
            (void)fprintf(stderr, "invalid options: %s\n", err);
            return EXIT_FAILURE;
        }
    }

    if (ctx.help) {
        return help();
    }

    if (ctx.test) {
        return test();
    }

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

    return run(&ctx, &timer, &ccodoc);
}

static const char* read_arg(int* i, const char** argv)
{
    const int next_i = *i + 1;
    const char* next_arg = argv[next_i];
    if (!next_arg) {
        return NULL;
    }

    *i = next_i;
    return next_arg;
}

static const char* configure_with_args(ccodoc_context* ctx, int argc, const char** argv)
{
    for (int i = 1; i < argc; i++) {
        const char* arg = argv[i];

        if (str_equals_to(arg, "--help")) {
            ctx->help = true;
        }

        if (str_equals_to(arg, "--debug")) {
            ctx->debug = true;
        }

        if (str_equals_to(arg, "--test")) {
            ctx->test = true;
        }

        if (str_equals_to(arg, "--duration")) {
            const char* raw = read_arg(&i, argv);
            if (raw == NULL) {
                return "duration: the value should be provided";
            }

            moment m = { 0 };
            // NOLINTNEXTLINE(cert-err34-c)
            (void)sscanf(raw, "%d:%d", &m.hours, &m.mins);

            const duration d = duration_from_moment(m);
            if (d.msecs != 0) {
                ctx->duration = d;
            }
        }
    }

    return NULL;
}

static void print_arg_help(const char* arg, const char* description)
{
    printf("%s\n", arg);
    printf("  %s\n\n", description);
}

static int help(void)
{
    printf("# ccodoc\n");
    printf("timer with ccodoc（ししおどし）\n");

    printf("\n## options\n");
    print_arg_help("--help", "Print help.");
    print_arg_help("--debug", "Show debug info.");
    print_arg_help("--duration HH:mm", "Set the timer for this duration. (default: 00:30)");

    return EXIT_SUCCESS;
}

static int run(const ccodoc_context* ctx, timer* timer, ccodoc* ccodoc)
{
    static const duration delta = {
        .msecs = 1000 / 24,
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

    return EXIT_SUCCESS;
}

#define EXPECT_PASS(label, actual)      \
    {                                   \
        printf(label);                  \
        if ((actual) != EXIT_SUCCESS) { \
            return EXIT_FAILURE;        \
        }                               \
    }

static int test(void)
{
    EXPECT_PASS("# ccodoc\n", test_ccodoc());
    printf("\n");

    EXPECT_PASS("# string\n", test_str());
    printf("\n");

    EXPECT_PASS("# time\n", test_time());
    printf("\n");

    printf("ALL PASS\n");

    return EXIT_SUCCESS;
}
