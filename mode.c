#include "mode.h"

#include "ccodoc.h"
#include "platform.h"
#include "renderer.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "assets/sounds/sounds.h"

typedef bool (*mode_processor_t)(ccodoc_mode_t*, duration_t);

typedef struct {
    ccodoc_mode_t* mode;
    duration_t delta;
} rendering_ctx_t;

static void init_ccodoc(ccodoc_mode_t* mode);
static void deinit_ccodoc(ccodoc_mode_t* mode);

static void init_renderer(ccodoc_mode_t* mode);
static void deinit_renderer(ccodoc_mode_t* mode);

static void init_sound(ccodoc_mode_t* mode);
static void deinit_sound(ccodoc_mode_t* mode);

static void run_mode(ccodoc_mode_t* mode, mode_processor_t processor);

static bool process_mode_wabi(ccodoc_mode_t*, duration_t delta);
static void render_wabi(rendering_ctx_t* ctx);

static bool process_mode_sabi(ccodoc_mode_t*, duration_t delta);
static void render_sabi(rendering_ctx_t* ctx);

static drawing_ctx_t make_drawing_ctx_center(const canvas_t* canvas);

static void play_sound(const char* file);

void init_mode(ccodoc_mode_t* const mode)
{
    init_renderer(mode);
    init_sound(mode);

    init_ccodoc(mode);
}

void deinit_mode(ccodoc_mode_t* const mode)
{
    deinit_ccodoc(mode);
    deinit_renderer(mode);
    deinit_sound(mode);
}

static void init_ccodoc(ccodoc_mode_t* const mode)
{
    mode->ccodoc = (ccodoc_t) {
        .kakehi = {
            .release_water_amount = 1,
            .holding_water = {
                .duration = { .msecs = 2200 },
            },
            .releasing_water = {
                .duration = { .msecs = 800 },
            },
        },
        .tsutsu = {
            .water_capacity = 10,
            .releasing_water = {
                .duration = { .msecs = 1200 },
            },
        },
        .hachi = {
            .releasing_water = {
                .duration = { .msecs = 1000 },
            },
        },
    };

    if (!mode->ornamental) {
        return;
    }

    {
        const char* file = mode->sound.tsutsu_drip;
        if (file != NULL) {
            mode->ccodoc.tsutsu.on_got_drip = (event_t) {
                .listener = (void*)file,
                .listen = (event_listener_t)play_sound,
            };
        }
    }
    {
        const char* file = mode->sound.tsutsu_bump;
        if (file != NULL) {
            mode->ccodoc.tsutsu.on_bumped = (event_t) {
                .listener = (void*)file,
                .listen = (event_listener_t)play_sound,
            };
        }
    }
}

static void deinit_ccodoc(ccodoc_mode_t* const mode)
{
    mode->ccodoc.tsutsu.on_got_drip = (event_t) { 0 };
    mode->ccodoc.tsutsu.on_bumped = (event_t) { 0 };
}

static void init_renderer(ccodoc_mode_t* const mode)
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

static void deinit_renderer(ccodoc_mode_t* const mode)
{
    deinit_canvas(&mode->rendering.canvas.value);
}

static char* install_sound(const char* const name, const unsigned char* data, size_t len);

static void init_sound(ccodoc_mode_t* mode)
{
    if (!mode->ornamental) {
        return;
    }

    mode->sound.tsutsu_drip = mode->sound.tsutsu_drip == NULL
        ? install_sound("tsutsu_drip.mp3", sound_tsutsu_drip, sizeof(sound_tsutsu_drip))
        : format_str(mode->sound.tsutsu_drip);

    mode->sound.tsutsu_bump = mode->sound.tsutsu_bump == NULL
        ? install_sound("tsutsu_bump.mp3", sound_tsutsu_bump, sizeof(sound_tsutsu_bump))
        : format_str(mode->sound.tsutsu_bump);

    mode->sound.uguisu_call = mode->sound.uguisu_call == NULL
        ? install_sound("uguisu_call.mp3", sound_uguisu_call, sizeof(sound_uguisu_call))
        : format_str(mode->sound.uguisu_call);
}

static void deinit_sound(ccodoc_mode_t* mode)
{
    if (mode->sound.tsutsu_drip != NULL) {
        free((void*)mode->sound.tsutsu_drip);
    }

    if (mode->sound.tsutsu_bump != NULL) {
        free((void*)mode->sound.tsutsu_bump);
    }

    if (mode->sound.uguisu_call != NULL) {
        free((void*)mode->sound.uguisu_call);
    }
}

void run_mode_wabi(ccodoc_mode_t* const mode)
{
    run_mode(mode, process_mode_wabi);
}

void run_mode_sabi(ccodoc_mode_t* const mode)
{
    run_mode(mode, process_mode_sabi);
}

static void run_mode(ccodoc_mode_t* const mode, const mode_processor_t processor)
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

static bool process_mode_wabi(ccodoc_mode_t* const mode, const duration_t delta)
{
    tick_ccodoc(&mode->ccodoc, delta);

    {
        rendering_ctx_t ctx = { .mode = mode, .delta = delta };
        render_with(
            &mode->rendering.renderer,
            (event_t) {
                .listener = &ctx,
                .listen = (event_listener_t)render_wabi,
            }
        );
    }

    return true;
}

static void render_wabi(rendering_ctx_t* const ctx)
{
    drawing_ctx_t dctx = make_drawing_ctx_center(&ctx->mode->rendering.canvas.value);

    render_ccodoc(&ctx->mode->rendering.renderer, &dctx, &ctx->mode->ccodoc);

    if (ctx->mode->debug) {
        render_debug_info(&ctx->mode->rendering.renderer, ctx->delta, &ctx->mode->ccodoc, NULL);
    }
}

static bool process_mode_sabi(ccodoc_mode_t* const mode, const duration_t delta)
{
    const water_flow_state_t tsutsu_last_state = mode->ccodoc.tsutsu.state;

    tick_ccodoc(&mode->ccodoc, delta);
    tick_timer(&mode->timer, delta);

    {
        rendering_ctx_t ctx = { .mode = mode, .delta = delta };
        render_with(
            &mode->rendering.renderer,
            (event_t) {
                .listener = &ctx,
                .listen = (event_listener_t)render_sabi,
            }
        );
    }

    if (!timer_expires(&mode->timer)) {
        return true;
    }

    // Stop the water flow since kakehi has released last water drop to fill up tsutsu,
    mode->ccodoc.kakehi.disabled = mode->ccodoc.kakehi.state == releasing_water;
    // and wait for tsutsu to have released water.
    if (tsutsu_last_state != releasing_water || !action_has_finished(&mode->ccodoc.tsutsu.releasing_water)) {
        return true;
    }

    if (mode->ornamental && mode->sound.uguisu_call != NULL) {
        sleep_for((duration_t) { .msecs = 1750 });
        play_sound(mode->sound.uguisu_call);
    }

    return false;
}

static void render_sabi(rendering_ctx_t* const ctx)
{
    drawing_ctx_t dctx = make_drawing_ctx_center(&ctx->mode->rendering.canvas.value);

    render_ccodoc(&ctx->mode->rendering.renderer, &dctx, &ctx->mode->ccodoc);
    render_timer(&ctx->mode->rendering.renderer, &dctx, &ctx->mode->timer);

    if (ctx->mode->debug) {
        render_debug_info(&ctx->mode->rendering.renderer, ctx->delta, &ctx->mode->ccodoc, &ctx->mode->timer);
    }
}

static drawing_ctx_t make_drawing_ctx_center(const canvas_t* const canvas)
{
    static const vec2d_t ccodoc_size = {
        .x = 14,
        .y = 6,
    };

    const vec2d_t canvas_size = get_canvas_size(canvas);

    drawing_ctx_t ctx = {
        .origin = {
            .x = (canvas_size.x - ccodoc_size.x) / 2,
            .y = (canvas_size.y - ccodoc_size.y) / 2,
        },
    };
    ctx.current = ctx.origin;

    return ctx;
}

static void play_sound(const char* const name)
{
#if PLATFORM != PLATFORM_MACOS
    (void)name;
    return;
#endif

    run_cmd("/usr/bin/afplay", (const char*[]) { "afplay", (char* const)name, NULL });
}

static char* install_sound(const char* const name, const unsigned char* const data, size_t len)
{
#if PLATFORM == PLATFORM_LINUX
    const char* path = join_paths((const char*[]) { user_cache_dir(), "ccodoc/assets/sounds", name, NULL });
#elif PLATFORM == PLATFORM_MACOS
    const char* path = join_paths((const char*[]) { user_cache_dir(), "ccodoc/assets/sounds", name, NULL });
#else
    (void)name;
    (void)data;
    (void)len;
    return NULL;
#endif

#if PLATFORM == PLATFORM_LINUX || PLATFORM == PLATFORM_MACOS
    if (path == NULL) {
        return NULL;
    }

    if (!has_file(path)) {
        FILE* file = fopen(path, "w");
        if (file == NULL) {
            free((void*)path);
            return NULL;
        }

        size_t n = fwrite(data, sizeof(unsigned char), len, file);
        if (n < len) {
            free((void*)path);
            return NULL;
        }

        (void)fclose(file);
    }

    return (char*)path;
#endif
}
