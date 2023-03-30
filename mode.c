#include "mode.h"

#include "ccodoc.h"
#include "platform.h"
#include <signal.h>

typedef bool (*mode_processor_t)(mode_t*, duration_t);

static void run_mode(mode_t* mode, mode_processor_t processor);
static bool process_mode_wabi(mode_t*, duration_t delta);
static bool process_mode_sabi(mode_t*, duration_t delta);

static void init_ccodoc(mode_t* mode);
static void deinit_ccodoc(mode_t* mode);

static void init_rendering_ctx(mode_t* mode);
static void deinit_rendering_ctx(mode_t* mode);

static void play_sound(const char* file);

void init_mode(mode_t* const mode)
{
    init_ccodoc(mode);
    init_rendering_ctx(mode);
}

void deinit_mode(mode_t* const mode)
{
    deinit_ccodoc(mode);
    deinit_rendering_ctx(mode);
}

void run_mode_wabi(mode_t* const mode)
{
    run_mode(mode, process_mode_wabi);
}

void run_mode_sabi(mode_t* const mode)
{
    run_mode(mode, process_mode_sabi);
}

static void run_mode(mode_t* const mode, const mode_processor_t processor)
{
    static const duration_t min_delta = { .msecs = 1000 / 24 };

    duration_t last_time = monotonic_time();

    while (1) {
        const duration_t time = monotonic_time();

        const duration_t delta = duration_diff(time, last_time);
        last_time = time;

        const bool continues = processor(mode, delta);
        if (!continues) {
            break;
        }

        const duration_t process_time = duration_diff(monotonic_time(), time);

        sleep_for(duration_diff(min_delta, process_time));
    }

    sigset_t sigs = { 0 };
    sigemptyset(&sigs);

    sigsuspend(&sigs);
}

static bool process_mode_wabi(mode_t* const mode, const duration_t delta)
{
    tick_ccodoc(&mode->ccodoc, delta);

    render(&mode->rendering.renderer, delta, &mode->ccodoc, NULL);

    return true;
}

static bool process_mode_sabi(mode_t* const mode, const duration_t delta)
{
    const water_flow_state_t tsutsu_last_state = mode->ccodoc.tsutsu.state;

    tick_ccodoc(&mode->ccodoc, delta);
    tick_timer(&mode->timer, delta);

    render(&mode->rendering.renderer, delta, &mode->ccodoc, &mode->timer);

    if (!timer_expires(&mode->timer)) {
        return true;
    }

    // Stop the water flow since kakehi has released last water drop to fill up tsutsu,
    mode->ccodoc.kakehi.disabled = mode->ccodoc.kakehi.state == releasing_water;
    // and wait for tsutsu to have released water.
    if (tsutsu_last_state != releasing_water || !tsutsu_has_released_water(&mode->ccodoc.tsutsu)) {
        return true;
    }

    if (mode->ornamental && mode->sound.uguisu_call != NULL) {
        sleep_for((duration_t) { .msecs = 2000 });
        play_sound(mode->sound.uguisu_call);
    }

    return false;
}

static void init_ccodoc(mode_t* const mode)
{
    mode->ccodoc = (ccodoc_t) {
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

    if (!mode->ornamental) {
        return;
    }

    {
        const char* file = mode->sound.tsutsu_poured;
        if (file != NULL) {
            mode->ccodoc.tsutsu.on_poured = (event_t) {
                .listener = (void*)file,
                .notify = (void_callback_t)play_sound,
            };
        }
    }
    {
        const char* file = mode->sound.tsutsu_bumped;
        if (file != NULL) {
            mode->ccodoc.tsutsu.on_released_water = (event_t) {
                .listener = (void*)file,
                .notify = (void_callback_t)play_sound,
            };
        }
    }
}

static void deinit_ccodoc(mode_t* const mode)
{
    mode->ccodoc.tsutsu.on_poured = (event_t) { 0 };
    mode->ccodoc.tsutsu.on_released_water = (event_t) { 0 };
}

static void init_rendering_ctx(mode_t* const mode)
{
    init_canvas_curses(&mode->rendering.canvas.delegate);
    init_canvas_proxy(&mode->rendering.canvas.proxy, &mode->rendering.canvas.delegate);

    mode->rendering.canvas.value = wrap_canvas_proxy(&mode->rendering.canvas.proxy);

    mode->rendering.renderer = (renderer_t) {
        .canvas = &mode->rendering.canvas.value,
        .ornamental = mode->ornamental,
        .debug = mode->debug,
    };
}

static void deinit_rendering_ctx(mode_t* const mode)
{
    deinit_renderer(&mode->rendering.renderer);
}

static void play_sound(const char* const name)
{
#if PLATFORM == PLATFORM_LINUX
    (void)name;
#endif
#if PLATFORM == PLATFORM_MACOS
    const char* const args[] = { "afplay", (char* const)name, NULL };
    run_cmd("/usr/bin/afplay", args);
#endif
}
