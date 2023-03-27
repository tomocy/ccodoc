#include "canvas.h"
#include "engine.h"
#include "renderer.h"
#include "string.h"
#include "time.h"
#include <stdlib.h>

typedef enum {
    mode_type_timer,
} mode_type_t;

typedef struct {
    mode_type_t type;

    struct {
        renderer_t* renderer;
        canvas_t* canvas;
    } rendering_ctx;

    union {
        struct {
            ccodoc_t* ccodoc;
            tick_timer_t* timer;
        } timer;
    } target;
} mode_t;

typedef struct {
    duration_t duration;
    bool decorative;

    struct {
        const char* tsutsu_poured;
        const char* tsutsu_bumped;
    } sound;

    bool help;
    bool debug;
} config_t;

typedef void (*processor_t)(void*, duration_t);

static const char* configure_with_args(config_t* config, int argc, const char* const* argv);
static int help(void);

static int run_timer(mode_t* mode);
static void process_timer(void* mode, duration_t delta);

static void process(void* processor, processor_t f);

int main(const int argc, const char* const* const argv)
{
    config_t config = {
        .duration = duration_from_moment((moment_t) { .mins = 30 }),
        .decorative = true,
        .help = false,
        .debug = false,
    };

    {
        const char* const err = configure_with_args(&config, argc, argv);
        if (err != NULL) {
            help();
            printf("\n");
            (void)fprintf(stderr, "invalid options: %s\n", err);
            return EXIT_FAILURE;
        }
    }

    if (config.help) {
        return help();
    }

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

    tick_timer_t timer = { .duration = config.duration };

    renderer_t renderer = {
        .decorative = config.decorative,
        .debug = config.debug,

        .sound = {
            .tsutsu_poured = config.sound.tsutsu_poured,
            .tsutsu_bumped = config.sound.tsutsu_bumped,
        },
    };

    canvas_curses_t canvas_delegate = { 0 };
    init_canvas_curses(&canvas_delegate, config.decorative);

    canvas_proxy_t canvas_proxy = { 0 };
    init_canvas_proxy(&canvas_proxy, &canvas_delegate);

    canvas_t canvas = wrap_canvas_proxy(&canvas_proxy);

    mode_t mode = {
        .type = mode_type_timer,
        .rendering_ctx = {
            .renderer = &renderer,
            .canvas = &canvas,
        },
        .target = {
            .timer = {
                .ccodoc = &ccodoc,
                .timer = &timer,
            },
        },
    };

    return run_timer(&mode);
}

static const char* read_arg(int* const i, const char* const* const argv)
{
    const int next_i = *i + 1;
    const char* next_arg = argv[next_i];
    if (!next_arg) {
        return NULL;
    }

    *i = next_i;
    return next_arg;
}

#define CONFIG_ERR_NO_VALUE_SPECIFIED(label) label ": value must be specified"

static const char* configure_with_args(config_t* const config, const int argc, const char* const* const argv)
{
    for (int i = 1; i < argc; i++) {
        const char* const arg = argv[i];

        if (str_equals(arg, "--timer")) {
            const char* const raw = read_arg(&i, argv);
            if (raw == NULL) {
                return CONFIG_ERR_NO_VALUE_SPECIFIED("timer");
            }

            moment_t m = { 0 };
            // NOLINTNEXTLINE(cert-err34-c)
            (void)sscanf(raw, "%d:%d", &m.hours, &m.mins);

            const duration_t d = duration_from_moment(m);
            if (d.msecs == 0) {
                return "timer: duration format must be HH:mm";
            }
            config->duration = d;

            continue;
        }

        if (str_equals(arg, "--plain")) {
            config->decorative = false;
            continue;
        }

        if (str_equals(arg, "--sound-tsutsu-poured")) {
            const char* const name = read_arg(&i, argv);
            if (name == NULL) {
                return CONFIG_ERR_NO_VALUE_SPECIFIED("sound-tsutsu-poured");
            }

            config->sound.tsutsu_poured = name;

            continue;
        }

        if (str_equals(arg, "--sound-tsutsu-bumped")) {
            const char* const name = read_arg(&i, argv);
            if (name == NULL) {
                return CONFIG_ERR_NO_VALUE_SPECIFIED("sound-tsutsu-bumped");
            }

            config->sound.tsutsu_bumped = name;

            continue;
        }

        if (str_equals(arg, "--help")) {
            config->help = true;
            continue;
        }

        if (str_equals(arg, "--debug")) {
            config->debug = true;
            continue;
        }
    }

    return NULL;
}

static void print_arg_help(const char* const arg, const char* const description)
{
    printf("%s\n", arg);
    printf("  %s\n\n", description);
}

static int help(void)
{
    printf("# ccodoc（鹿威し）\n");

    printf("\n## options\n");
    print_arg_help("--timer HH:mm", "Set the timer for this duration. (default: 00:30)");

    print_arg_help("--plain", "Render ccodoc without decoration.");
    print_arg_help("--sound-tsutsu-poured", "Play this sound on tsutsu（筒）poured");
    print_arg_help("--sound-tsutsu-bumped", "Play this sound on tsutsu（筒）bumped");

    print_arg_help("--help", "Print help.");

    return EXIT_SUCCESS;
}

static int run_timer(mode_t* mode)
{
    init_renderer(
        mode->rendering_ctx.renderer, mode->rendering_ctx.canvas,
        mode->target.timer.ccodoc
    );

    process(mode, process_timer);

    deinit_renderer(mode->rendering_ctx.renderer, mode->target.timer.ccodoc);

    return EXIT_SUCCESS;
}

static void process_timer(void* raw_mode, duration_t delta)
{
    mode_t* const mode = raw_mode;

    tick_ccodoc(mode->target.timer.ccodoc, delta);
    tick_timer(mode->target.timer.timer, delta);

    render(
        mode->rendering_ctx.renderer,
        delta,
        mode->target.timer.ccodoc, mode->target.timer.timer
    );
}

static void process(void* const processor, processor_t const f)
{
    static const duration_t min_delta = { .msecs = 1000 / 24 };

    duration_t last_time = monotonic_time();

    while (1) {
        const duration_t time = monotonic_time();

        const duration_t delta = duration_diff(time, last_time);
        last_time = time;

        f(processor, delta);

        const duration_t process_time = duration_diff(monotonic_time(), time);

        sleep_for(duration_diff(min_delta, process_time));
    }
}
