#include "canvas.h"
#include "ccodoc.h"
#include "renderer.h"
#include "string.h"
#include "time.h"
#include <stdlib.h>

typedef enum {
    mode_wabi,
    mode_sabi,
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
        } wabi;

        struct {
            ccodoc_t* ccodoc;
            tick_timer_t* timer;
        } sabi;
    } target;
} mode_t;

typedef struct {
    mode_type_t mode;

    struct {
        duration_t duration;
    } sabi;

    bool satori;

    struct {
        const char* tsutsu_poured;
        const char* tsutsu_bumped;
    } sound;

    bool help;
    bool debug;
} config_t;

typedef void (*mode_processor_t)(mode_t*, duration_t);

static const char* configure_with_args(config_t* config, int argc, const char* const* argv);
static int help(void);

static int run(mode_t* mode, mode_processor_t processor);

static void process_wabi(mode_t* mode, duration_t delta);
static void process_sabi(mode_t* mode, duration_t delta);

static void process(mode_t* mode, mode_processor_t processor);

int main(const int argc, const char* const* const argv)
{
    config_t config = {
        .mode = mode_wabi,
        .satori = false,
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

    renderer_t renderer = {
        .ornamental = !config.satori,
        .debug = config.debug,

        .sound = {
            .tsutsu_poured = config.sound.tsutsu_poured,
            .tsutsu_bumped = config.sound.tsutsu_bumped,
        },
    };

    canvas_curses_t canvas_delegate = { 0 };
    init_canvas_curses(&canvas_delegate, !config.satori);

    canvas_proxy_t canvas_proxy = { 0 };
    init_canvas_proxy(&canvas_proxy, &canvas_delegate);

    canvas_t canvas = wrap_canvas_proxy(&canvas_proxy);

    mode_t mode = {
        .type = mode_sabi,
        .rendering_ctx = {
            .renderer = &renderer,
            .canvas = &canvas,
        },
    };

    mode_processor_t processor = NULL;

    switch (config.mode) {
    case mode_wabi:
        mode.target.wabi.ccodoc = &ccodoc;
        processor = process_wabi;
        break;
    case mode_sabi:
        mode.target.sabi.ccodoc = &ccodoc;

        tick_timer_t timer = { .duration = config.sabi.duration };
        mode.target.sabi.timer = &timer;

        processor = process_sabi;
        break;
    }

    return run(&mode, processor);
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

        if (str_equals(arg, "--sabi")) {
            const char* const raw = read_arg(&i, argv);
            if (raw == NULL) {
                return CONFIG_ERR_NO_VALUE_SPECIFIED("sabi");
            }

            moment_t m = { 0 };
            // NOLINTNEXTLINE(cert-err34-c)
            (void)sscanf(raw, "%d:%d", &m.hours, &m.mins);

            const duration_t d = duration_from_moment(m);
            if (d.msecs == 0) {
                return "timer: duration format must be HH:mm";
            }

            config->mode = mode_sabi;
            config->sabi.duration = d;

            continue;
        }

        if (str_equals(arg, "--satori")) {
            config->satori = true;
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

static void print_arg_help(const char* const arg, const char** const descs)
{
    printf("%s\n", arg);

    for (const char** desc = descs; *desc; desc++) {
        printf("  %s\n", *desc);
    }
    printf("\n");
}

static int help(void)
{
    printf("# ccodoc\n");

    printf("\nccodoc（鹿威し）simulated（造）in your terminal\n");

    printf("\n\n## options\n\n");

    print_arg_help(
        "--wabi",
        (const char*[]) {
            "Render ccodoc alone. (default)",
            "Imperfect does not always mean \"not perfect\". Simple does not always mean \"not complicated\".",
            "It is just what it is.（侘び）",
            NULL,
        }
    );

    print_arg_help(
        "--sabi HH:mm",
        (const char*[]) {
            "Render ccodoc with the timer set for this duration.",
            "Impermanent does not always mean \"not permanent\". Behind does not always mean \"not ahead\".",
            "It will be just what it will be.（寂び）",
            NULL,
        }
    );

    print_arg_help(
        "--satori",
        (const char*[]) {
            "Remove all ornaments.",
            "Neither color nor sound is needed as they already lies within you.（悟り）",
            NULL,
        }
    );

    print_arg_help(
        "--sound-tsutsu-poured file",
        (const char*[]) { "Play this sound on tsutsu（筒）poured.", NULL }
    );
    print_arg_help(
        "--sound-tsutsu-bumped file",
        (const char*[]) { "Play this sound on tsutsu（筒）bumped.", NULL }
    );

    print_arg_help("--help", (const char*[]) { "Print help.", NULL });

    return EXIT_SUCCESS;
}

static int run(mode_t* const mode, mode_processor_t const processor)
{
    init_renderer(
        mode->rendering_ctx.renderer, mode->rendering_ctx.canvas,
        mode->target.sabi.ccodoc
    );

    process(mode, processor);

    deinit_renderer(mode->rendering_ctx.renderer, mode->target.sabi.ccodoc);

    return EXIT_SUCCESS;
}

static void process_wabi(mode_t* mode, duration_t delta)
{
    tick_ccodoc(mode->target.sabi.ccodoc, delta);

    render(
        mode->rendering_ctx.renderer,
        delta,
        mode->target.sabi.ccodoc, NULL
    );
}

static void process_sabi(mode_t* const mode, const duration_t delta)
{
    tick_ccodoc(mode->target.sabi.ccodoc, delta);
    tick_timer(mode->target.sabi.timer, delta);

    render(
        mode->rendering_ctx.renderer,
        delta,
        mode->target.sabi.ccodoc, mode->target.sabi.timer
    );
}

static void process(mode_t* mode, mode_processor_t processor)
{
    static const duration_t min_delta = { .msecs = 1000 / 24 };

    duration_t last_time = monotonic_time();

    while (1) {
        const duration_t time = monotonic_time();

        const duration_t delta = duration_diff(time, last_time);
        last_time = time;

        processor(mode, delta);

        const duration_t process_time = duration_diff(monotonic_time(), time);

        sleep_for(duration_diff(min_delta, process_time));
    }
}
