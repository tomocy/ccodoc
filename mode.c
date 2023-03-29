#include "mode.h"

#include "platform.h"

typedef void (*mode_processor_t)(void*, duration_t);

static void init_mode_wabi(mode_wabi_t* mode, const mode_opt_wabi_t* opt);
static void init_mode_sabi(mode_sabi_t* mode, const mode_opt_sabi_t* opt);

static void deinit_mode_wabi(mode_wabi_t* mode);
static void deinit_mode_sabi(mode_sabi_t* mode);

static void process_mode_wabi(void* mode, duration_t delta);
static void process_mode_sabi(void* mode, duration_t delta);

static void init_ccodoc(ccodoc_t* ccodoc, const mode_opt_general_t* opt);
static void deinit_ccodoc(ccodoc_t* ccodoc);

static void init_rendering_ctx(rendering_ctx_t* ctx, const mode_opt_general_t* opt);
static void deinit_rendering_ctx(rendering_ctx_t* ctx);

static void play_sound(const char* file);

void init_mode(mode_t* const mode, const mode_opt_t opt)
{
    switch (mode->type) {
    case mode_wabi:
        init_mode_wabi(&mode->delegate.wabi, &opt.delegate.wabi);
        break;
    case mode_sabi:
        init_mode_sabi(&mode->delegate.sabi, &opt.delegate.sabi);
        break;
    }
}

void deinit_mode(mode_t* const mode)
{
    switch (mode->type) {
    case mode_wabi:
        deinit_mode_wabi(&mode->delegate.wabi);
        break;
    case mode_sabi:
        deinit_mode_sabi(&mode->delegate.sabi);
        break;
    }
}

void run_mode(mode_t* mode)
{
    void* raw_mode = NULL;
    mode_processor_t processor = NULL;

    switch (mode->type) {
    case mode_wabi:
        raw_mode = &mode->delegate.wabi;
        processor = process_mode_wabi;
        break;
    case mode_sabi:
        raw_mode = &mode->delegate.sabi;
        processor = process_mode_sabi;
        break;
    }

    static const duration_t min_delta = { .msecs = 1000 / 24 };

    duration_t last_time = monotonic_time();

    while (1) {
        const duration_t time = monotonic_time();

        const duration_t delta = duration_diff(time, last_time);
        last_time = time;

        processor(raw_mode, delta);

        const duration_t process_time = duration_diff(monotonic_time(), time);

        sleep_for(duration_diff(min_delta, process_time));
    }
}

// wabi

static void init_mode_wabi(mode_wabi_t* const mode, const mode_opt_wabi_t* opt)
{
    init_ccodoc(&mode->ccodoc, &opt->general);
    init_rendering_ctx(&mode->rendering_ctx, &opt->general);
}

static void deinit_mode_wabi(mode_wabi_t* const mode)
{
    deinit_ccodoc(&mode->ccodoc);
    deinit_rendering_ctx(&mode->rendering_ctx);
}

static void process_mode_wabi(void* const raw_mode, const duration_t delta)
{
    mode_wabi_t* mode = raw_mode;

    tick_ccodoc(&mode->ccodoc, delta);

    render(&mode->rendering_ctx.renderer, delta, &mode->ccodoc, NULL);
}

// sabi

static void init_mode_sabi(mode_sabi_t* const mode, const mode_opt_sabi_t* opt)
{
    init_ccodoc(&mode->ccodoc, &opt->general);

    mode->timer = (tick_timer_t) { .duration = opt->duration };

    init_rendering_ctx(&mode->rendering_ctx, &opt->general);
}

static void deinit_mode_sabi(mode_sabi_t* const mode)
{
    deinit_ccodoc(&mode->ccodoc);
    deinit_rendering_ctx(&mode->rendering_ctx);
}

static void process_mode_sabi(void* const raw_mode, const duration_t delta)
{
    mode_sabi_t* mode = raw_mode;

    tick_ccodoc(&mode->ccodoc, delta);
    tick_timer(&mode->timer, delta);

    render(&mode->rendering_ctx.renderer, delta, &mode->ccodoc, &mode->timer);
}

static void init_ccodoc(ccodoc_t* ccodoc, const mode_opt_general_t* opt)
{
    *ccodoc = (ccodoc_t) {
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

    if (!opt->ornamental) {
        return;
    }

    {
        const char* file = opt->sound.tsutsu_poured;
        if (file != NULL) {
            ccodoc->tsutsu.on_poured = (event_t) {
                .listener = (void*)file,
                .notify = (void_callback_t)play_sound,
            };
        }
    }
    {
        const char* file = opt->sound.tsutsu_bumped;
        if (file != NULL) {
            ccodoc->tsutsu.on_released_water = (event_t) {
                .listener = (void*)file,
                .notify = (void_callback_t)play_sound,
            };
        }
    }
}

static void deinit_ccodoc(ccodoc_t* ccodoc)
{
    ccodoc->tsutsu.on_poured = (event_t) { 0 };
    ccodoc->tsutsu.on_released_water = (event_t) { 0 };
}

static void init_rendering_ctx(rendering_ctx_t* const ctx, const mode_opt_general_t* opt)
{
    init_canvas_curses(&ctx->canvas.delegate, opt->ornamental);
    init_canvas_proxy(&ctx->canvas.proxy, &ctx->canvas.delegate);

    ctx->canvas.value = wrap_canvas_proxy(&ctx->canvas.proxy);

    ctx->renderer = (renderer_t) {
        .ornamental = opt->ornamental,
        .debug = opt->debug,
    };

    init_renderer(&ctx->renderer, &ctx->canvas.value);
}

static void deinit_rendering_ctx(rendering_ctx_t* const ctx)
{
    deinit_renderer(&ctx->renderer);
}

// sound

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
