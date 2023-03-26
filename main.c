#include "canvas.h"
#include "engine.h"
#include "renderer.h"
#include "string.h"
#include "time.h"
#include <stdlib.h>

static const char* configure_with_args(context_t* ctx, int argc, const char** argv);
static int help(void);
static int run(const context_t* ctx, tick_timer_t* timer, ccodoc_t* ccodoc);

int main(int argc, const char** argv)
{
    context_t ctx = {
        .decorative = false,
        .debug = false,
        .duration = duration_from_moment((moment_t) { .mins = 30 }),
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

    tick_timer_t timer = { .duration = ctx.duration };

    ccodoc_t ccodoc = {
        .kakehi = {
            .release_water_ratio = 0.1f,
            .holding_water_timer = {
                .duration = { .msecs = 2200 },
            },
            .releasing_water_timer = {
                .duration = { .msecs = 800 },
            },
        },
        .tsutsu = {
            .water_capacity = 10,
            .releasing_water_timer = {
                .duration = { .msecs = 1200 },
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

static const char*
configure_with_args(context_t* ctx, int argc, const char** argv)
{
    for (int i = 1; i < argc; i++) {
        const char* arg = argv[i];

        if (str_equals(arg, "--decorative")) {
            ctx->decorative = true;
        }

        if (str_equals(arg, "--duration")) {
            const char* raw = read_arg(&i, argv);
            if (raw == NULL) {
                return "duration: the value must be specified";
            }

            moment_t m = { 0 };
            // NOLINTNEXTLINE(cert-err34-c)
            (void)sscanf(raw, "%d:%d", &m.hours, &m.mins);

            const duration_t d = duration_from_moment(m);
            if (d.msecs == 0) {
                return "duration: format must be HH:mm";
            }
            ctx->duration = d;
        }

        if (str_equals(arg, "--help")) {
            ctx->help = true;
        }

        if (str_equals(arg, "--debug")) {
            ctx->debug = true;
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
    printf("timer with ccodoc（鹿威し）\n");

    printf("\n## options\n");
    print_arg_help("--decorative", "Render ccodoc with decoration.");
    print_arg_help("--duration HH:mm", "Set the timer for this duration. (default: 00:30)");
    print_arg_help("--help", "Print help.");

    return EXIT_SUCCESS;
}

static int run(const context_t* ctx, tick_timer_t* timer, ccodoc_t* ccodoc)
{
    renderer_t renderer = { 0 };

    canvas_curses_t canvas_delegate = { 0 };
    init_canvas_curses(&canvas_delegate, ctx->decorative);

    canvas_proxy_t canvas_proxy = { 0 };
    init_canvas_proxy(&canvas_proxy, &canvas_delegate);

    canvas_t canvas = wrap_canvas_proxy(&canvas_proxy);

    init_renderer(&renderer, &canvas, ccodoc);

    {
        static const duration_t min_delta = { .msecs = 1000 / 24 };

        duration_t last_time = monotonic_time();

        while (1) {
            const duration_t time = monotonic_time();

            const duration_t delta = duration_diff(time, last_time);
            last_time = time;

            {
                tick_timer(timer, delta);
                tick_ccodoc(ccodoc, delta);

                render_ccodoc(&renderer, delta, ctx, timer, ccodoc);
            }

            const duration_t process_time = duration_diff(monotonic_time(), time);

            const duration_t sleep_time = duration_diff(min_delta, process_time);
            sleep_for(sleep_time);
        }
    }

    deinit_renderer(&renderer, ccodoc);

    return EXIT_SUCCESS;
}
