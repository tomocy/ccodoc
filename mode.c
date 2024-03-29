#include "mode.h"

#include "ccodoc.h"
#include "platform.h"
#include "renderer.h"
#include "time.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "assets/sounds/sounds.h"

typedef bool (*process_mode_t)(struct mode*, struct duration);

static void init_ccodoc(struct mode* mode);
static void deinit_ccodoc(struct mode* mode);

static void init_renderer(struct mode* mode);
static void deinit_renderer(struct mode* mode);

static void init_sound(struct mode* mode);
static void deinit_sound(struct mode* mode);

static void run_mode(const struct mode_ctx* ctx, struct mode* mode, process_mode_t process);

static bool process_wabi(struct mode*, struct duration delta);
static bool process_sabi(struct mode*, struct duration delta);

static struct drawing_ctx make_drawing_ctx_center(const struct canvas* canvas);

static void play_sound(const char* file);

void init_mode(struct mode* const mode)
{
    init_renderer(mode);
    init_sound(mode);

    init_ccodoc(mode);
}

void deinit_mode(struct mode* const mode)
{
    deinit_ccodoc(mode);
    deinit_renderer(mode);
    deinit_sound(mode);
}

static void init_ccodoc(struct mode* const mode)
{
    mode->ccodoc = (struct ccodoc) {
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
            mode->ccodoc.tsutsu.on_got_drip = (struct event) {
                .listener = (void*)file,
                .listen = (event_listener_t)play_sound,
            };
        }
    }
    {
        const char* file = mode->sound.tsutsu_bump;
        if (file != NULL) {
            mode->ccodoc.tsutsu.on_bumped = (struct event) {
                .listener = (void*)file,
                .listen = (event_listener_t)play_sound,
            };
        }
    }
}

static void deinit_ccodoc(struct mode* const mode)
{
    mode->ccodoc.tsutsu.on_got_drip = (struct event) { 0 };
    mode->ccodoc.tsutsu.on_bumped = (struct event) { 0 };
}

static void init_renderer(struct mode* const mode)
{
    init_canvas_curses(&mode->rendering.canvas.delegate);
    init_canvas_proxy(&mode->rendering.canvas.proxy, &mode->rendering.canvas.delegate);

    mode->rendering.canvas.value = wrap_canvas_proxy(&mode->rendering.canvas.proxy);

    mode->rendering.renderer = (struct renderer) {
        .canvas = &mode->rendering.canvas.value,
        .ornamental = mode->ornamental,
    };
}

static void deinit_renderer(struct mode* const mode)
{
    deinit_canvas(&mode->rendering.canvas.value);
}

static char* install_sound(const char* const name, const unsigned char* data, size_t len);

static void init_sound(struct mode* const mode)
{
    if (!mode->ornamental) {
        return;
    }

    mode->sound.tsutsu_drip = install_sound("tsutsu_drip.mp3", sound_tsutsu_drip, sizeof(sound_tsutsu_drip));
    mode->sound.tsutsu_bump = install_sound("tsutsu_bump.mp3", sound_tsutsu_bump, sizeof(sound_tsutsu_bump));
    mode->sound.uguisu_call = install_sound("uguisu_call.mp3", sound_uguisu_call, sizeof(sound_uguisu_call));
}

static void deinit_sound(struct mode* const mode)
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

void run_mode_wabi(const struct mode_ctx* const ctx, struct mode* const mode)
{
    run_mode(ctx, mode, process_wabi);
}

void run_mode_sabi(const struct mode_ctx* const ctx, struct mode* const mode)
{
    run_mode(ctx, mode, process_sabi);
}

static bool process_for(struct mode* mode, const process_mode_t process, struct duration min_delta, struct duration duration);

static void run_mode(const struct mode_ctx* const ctx, struct mode* const mode, const process_mode_t process)
{
    static const struct duration min_delta = { .msecs = 1000 / 25 };

    struct duration last_time = get_monotonic_time();

    while (true) {
        {
            unsigned int sig = 0;
            bool caught = false;
            const char* const err = catch_sig(ctx->sig_handler, &sig, &caught);
            if (err != NULL) {
                // Discard the error as signal handling is less critical than ccodoc or the main functionality,
                // and ccodoc works fine even without it at worst.
                free((void*)err);
            }
            if (caught) {
                return;
            }
        }

        const struct duration time = get_monotonic_time();

        const struct duration delta = duration_diff(time, last_time);
        last_time = time;

        const bool continues = process_for(mode, process, min_delta, delta);
        if (!continues) {
            break;
        }

        const struct duration process_time = duration_diff(get_monotonic_time(), time);

        sleep_for(duration_diff(min_delta, process_time));
    }

    sigset_t sigs = { 0 };
    sigemptyset(&sigs);

    sigsuspend(&sigs);
}

static bool process_for(
    struct mode* const mode, const process_mode_t process,
    const struct duration min_delta, const struct duration duration
)
{
    for (struct duration elapsed = { 0 }; elapsed.msecs < duration.msecs;) {
        const struct duration delta = (struct duration) {
            .msecs = MIN(min_delta.msecs, duration.msecs - elapsed.msecs),
        };

        const bool continues = process(mode, delta);
        if (!continues) {
            return false;
        }

        elapsed.msecs += delta.msecs;
    }

    return true;
}

static bool process_wabi(struct mode* const mode, const struct duration delta)
{
    tick_ccodoc(&mode->ccodoc, delta);

    RENDER(&mode->rendering.renderer, {
        struct drawing_ctx ctx = make_drawing_ctx_center(&mode->rendering.canvas.value);

        render_ccodoc(&mode->rendering.renderer, &ctx, &mode->ccodoc);

        if (mode->debug) {
            render_debug_info(&mode->rendering.renderer, delta, &mode->ccodoc, NULL);
        }
    });

    return true;
}

static bool process_sabi(struct mode* const mode, const struct duration delta)
{
    struct ccodoc* ccodoc = &mode->ccodoc;

    const enum water_flow_state tsutsu_last_state = ccodoc->tsutsu.state;

    tick_ccodoc(ccodoc, delta);
    tick_timer(&mode->timer, delta);

    RENDER(&mode->rendering.renderer, {
        struct drawing_ctx ctx = make_drawing_ctx_center(&mode->rendering.canvas.value);

        render_ccodoc(&mode->rendering.renderer, &ctx, &mode->ccodoc);

        ctx.current = vec2d_add(ctx.current, (struct vec2d) { .y = 4 });
        render_timer(&mode->rendering.renderer, &ctx, &mode->timer);

        if (mode->debug) {
            render_debug_info(&mode->rendering.renderer, delta, &mode->ccodoc, &mode->timer);
        }
    });

    // Stop the water flow now that the kakehi has released the last drop of water to fill up the tsutsu within the timer duration,
    ccodoc->kakehi.disabled = get_remaining_time(&mode->timer).msecs <= ccodoc->kakehi.releasing_water.duration.msecs
        && ccodoc->kakehi.state == releasing_water;

    if (!timer_expires(&mode->timer)) {
        return true;
    }

    // and wait for the tsutsu to release the water.
    if (tsutsu_last_state != releasing_water || !action_has_finished(&ccodoc->tsutsu.releasing_water)) {
        return true;
    }

    if (mode->ornamental && mode->sound.uguisu_call != NULL) {
        sleep_for((struct duration) { .msecs = 1750 });
        play_sound(mode->sound.uguisu_call);
    }

    return false;
}

static struct drawing_ctx make_drawing_ctx_center(const struct canvas* const canvas)
{
    static const struct vec2d ccodoc_size = {
        .x = 14,
        .y = 6,
    };

    const struct vec2d canvas_size = get_canvas_size(canvas);

    struct drawing_ctx ctx = {
        .origin = {
            .x = canvas_size.x > ccodoc_size.x ? (canvas_size.x - ccodoc_size.x) / 2 : 0,
            .y = canvas_size.y > ccodoc_size.y ? (canvas_size.y - ccodoc_size.y) / 2 : 0,
        },
    };
    ctx.current = ctx.origin;

    return ctx;
}

static void play_sound(const char* const name)
{
#if PLATFORM == PLATFORM_LINUX
    run_cmd("/usr/bin/mpg123", (const char*[]) { "mpg123", "--quiet", name, NULL });
#elif PLATFORM == PLATFORM_MACOS
    run_cmd("/usr/bin/afplay", (const char*[]) { "afplay", name, NULL });
#else
    (void)name;
    return;
#endif
}

static char* install_sound(const char* const name, const unsigned char* const data, const size_t len)
{
#if PLATFORM == PLATFORM_LINUX
    const char* const path = join_paths((const char*[]) { get_user_cache_dir(), "ccodoc/assets/sounds", name, NULL });
#elif PLATFORM == PLATFORM_MACOS
    const char* const path = join_paths((const char*[]) { get_user_cache_dir(), "ccodoc/assets/sounds", name, NULL });
#else
    (void)name;
    (void)data;
    (void)len;
    return NULL;
#endif

    if (path == NULL) {
        return NULL;
    }

    if (!has_file(path)) {
        {
            const char* const dir = get_dir(path);
            const char* const err = make_dir(dir);
            free((void*)dir);
            if (err != NULL) {
                return NULL;
            }
        }

        FILE* file = fopen(path, "w");
        if (file == NULL) {
            free((void*)path);
            return NULL;
        }

        const size_t n = fwrite(data, sizeof(unsigned char), len, file);
        if (n < len) {
            free((void*)path);
            return NULL;
        }

        (void)fclose(file);
    }

    return (char*)path;
}
