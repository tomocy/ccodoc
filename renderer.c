#include "renderer.h"

#include "canvas.h"
#include "math.h"
#include "string.h"
#include <assert.h>
#include <math.h>

static void on_tsutsu_poured(void* renderer);
static void on_tsutsu_released_water(void* renderer);

void init_renderer(renderer_t* const renderer, canvas_t* const canvas, ccodoc_t* const ccodoc)
{
    renderer->canvas = canvas;

    ccodoc->tsutsu.on_poured = (event_t) {
        .listener = renderer,
        .notify = on_tsutsu_poured,
    };
    ccodoc->tsutsu.on_released_water = (event_t) {
        .listener = renderer,
        .notify = on_tsutsu_released_water,
    };
}

void deinit_renderer(renderer_t* const renderer, ccodoc_t* const ccodoc)
{
    deinit_canvas(renderer->canvas);

    ccodoc->tsutsu.on_poured = (event_t) { 0 };
    ccodoc->tsutsu.on_released_water = (event_t) { 0 };
}

static void render_kakehi(renderer_t* renderer, drawing_context_t* ctx, const kakehi_t* kakehi);
static void render_tsutsu(renderer_t* renderer, drawing_context_t* ctx, const tsutsu_t* tsutsu);
static void render_hachi(renderer_t* renderer, drawing_context_t* ctx, const hachi_t* hachi);
static void render_roji(renderer_t* renderer, drawing_context_t* ctx);
static void render_timer(renderer_t* renderer, drawing_context_t* ctx, const tick_timer_t* timer);
static void render_debug_info(
    renderer_t* renderer,
    const drawing_context_t* ctx,
    duration_t delta, const tick_timer_t* timer, const ccodoc_t* ccodoc
);

void render_ccodoc(
    renderer_t* const renderer,
    const rendering_context_t* const ctx,
    const duration_t delta,
    const tick_timer_t* const timer,
    const ccodoc_t* const ccodoc
)
{
    static const vector2d_t ccodoc_size = {
        .x = 14,
        .y = 6,
    };

    const vector2d_t canvas_size = get_canvas_size(renderer->canvas);

    drawing_context_t dctx = {
        .decorative = ctx->decorative,
        .origin = {
            .x = (canvas_size.x - ccodoc_size.x) / 2,
            .y = (canvas_size.y - ccodoc_size.y) / 2,
        },
    };
    dctx.current = dctx.origin;

    clear_canvas(renderer->canvas);

    render_kakehi(renderer, &dctx, &ccodoc->kakehi);
    render_tsutsu(renderer, &dctx, &ccodoc->tsutsu);
    render_hachi(renderer, &dctx, &ccodoc->hachi);
    render_roji(renderer, &dctx);

    render_timer(renderer, &dctx, timer);

    if (ctx->debug) {
        render_debug_info(renderer, &dctx, delta, timer, ccodoc);
    }

    flush_canvas(renderer->canvas);
}

static void render_kakehi(renderer_t* const renderer, drawing_context_t* const ctx, const kakehi_t* const kakehi)
{
    const char* art = NULL;
    switch (kakehi->state) {
    case holding_water: {
        static const float holding_ratio_1 = 1.0f / 3 * 1;
        static const float holding_ratio_2 = 1.0f / 3 * 2;

        const float ratio = elapsed_time_ratio(&kakehi->holding_water_timer);

        if (0 <= ratio && ratio < holding_ratio_1) {
            art = "━══";
        } else if (holding_ratio_1 <= ratio && ratio < holding_ratio_2) {
            art = "═━═";
        } else {
            art = "══━";
        }

        break;
    }
    case releasing_water:
        art = "═══";
        break;
    }

    assert(art != NULL);

    if (ctx->decorative) {
        int i = 0;
        for (const char* c = art; *c;) {
            const char_descriptor_t desc = decode_char_utf8(c);
            const bool has_water = str_equals_n(c, "━", desc.len);

            drawf(
                renderer->canvas,
                vector2d_add(ctx->current, (vector2d_t) { .x = i }),
                (drawing_attr_t) {
                    .color = has_water ? color_blue : color_yellow,
                },
                "%.*s", desc.len, c
            );

            i++;
            c += desc.len;
        }
    } else {
        draw(renderer->canvas, ctx->current, ctx->attr, art);
    }

    wrap_drawing_lines(ctx, 1);
}

static void render_tsutsu(renderer_t* const renderer, drawing_context_t* const ctx, const tsutsu_t* const tsutsu)
{
    static const size_t art_height = 4;

    // jo (序)
    static const char* const art_jo[] = {
        "◥◣",
        "  ◥◣",
        "  ▕ ◥◣",
        "  ▕   ◥◣",
    };

    // ha (破)
    static const char* const art_ha[] = {
        "",
        "◢◤◢◤◢◤◢◤",
        "  ▕ ",
        "  ▕ ",
    };

    // kyu (急)
    static const char* const art_kyu[] = {
        "      ◢◤",
        "    ◢◤",
        "  ◢◤",
        "◢◤▕",
    };

    const float water_amount_ratio = tsutsu_water_amount_ratio(tsutsu);
    const char* const* art = NULL;

    switch (tsutsu->state) {
    case holding_water:
        if (water_amount_ratio < 0.8) {
            art = art_jo;
        } else if (water_amount_ratio < 1) {
            art = art_ha;
        } else {
            art = art_kyu;
        }

        break;
    case releasing_water: {
        const float ratio = elapsed_time_ratio(&tsutsu->releasing_water_timer);

        if (ratio < 0.55) {
            art = art_kyu;
        } else if (ratio < 1) {
            art = art_ha;
        } else {
            art = art_jo;
        }

        break;
    }
    }

    assert(art != NULL);

    vector2d_t origin = ctx->current;
    origin.x += 3;

    for (size_t h = 0; h < art_height; h++) {
        if (!ctx->decorative) {
            draw(
                renderer->canvas,
                vector2d_add(origin, (vector2d_t) { .y = h }),
                ctx->attr,
                art[h]
            );
            continue;
        }

        int i = 0;
        const char* c = art[h];
        while (*c) {
            const char_descriptor_t desc = decode_char_utf8(c);

            drawing_attr_t attr = (drawing_attr_t) { .color = color_white };

            if (
                str_equals_n(c, "◥", desc.len)
                || str_equals_n(c, "◣", desc.len)
                || str_equals_n(c, "◢", desc.len)
                || str_equals_n(c, "◤", desc.len)
            ) {
                attr.color = color_green;
            }

            if (str_equals_n(c, "▕", desc.len)) {
                attr.color = color_yellow;
            }

            drawf(
                renderer->canvas,
                vector2d_add(origin, (vector2d_t) { .y = h, .x = i }),
                attr,
                "%.*s", desc.len, c
            );

            i++;
            c += desc.len;
        }
    }

    wrap_drawing_lines(ctx, art_height);
}

static void render_hachi(renderer_t* const renderer, drawing_context_t* const ctx, const hachi_t* const hachi)
{
    static const unsigned int art_width = 4;

    const char* art = NULL;

    switch (hachi->state) {
    case holding_water:
        art = "▭▭▭▭";
        break;
    case releasing_water: {
        float ratio = elapsed_time_ratio(&hachi->releasing_water_timer);

        if (ratio < 0.35) {
            art = "▭▬▬▭";
        } else if (ratio < 0.65) {
            art = "▬▭▭▬";
        } else {
            art = "▭▭▭▭";
        }

        break;
    }
    }

    if (ctx->decorative) {
        int i = 0;
        const char* c = art;
        while (*c) {
            const char_descriptor_t desc = decode_char_utf8(c);
            const bool has_water = str_equals_n(c, "▬", desc.len);

            drawf(
                renderer->canvas,
                vector2d_add(ctx->current, (vector2d_t) { .x = i }),
                (drawing_attr_t) {
                    .color = has_water ? color_blue : color_grey,
                },
                "%.*s", desc.len, c
            );

            i++;
            c += desc.len;
        }
    } else {
        draw(renderer->canvas, ctx->current, ctx->attr, art);
    }

    ctx->current.x += art_width;
}

static void render_roji(renderer_t* const renderer, drawing_context_t* const ctx)
{
    draw(
        renderer->canvas,
        ctx->current,
        (drawing_attr_t) { .color = color_green, .dim = true },
        "━━━━━━"
    );
    ctx->current.x += 6;

    draw(
        renderer->canvas,
        ctx->current,
        (drawing_attr_t) { .color = color_grey },
        "▨▨▨▨"
    );

    wrap_drawing_lines(ctx, 1);
}

static void render_timer(renderer_t* const renderer, drawing_context_t* const ctx, const tick_timer_t* const timer)
{
    ctx->current.y += 4;

    {
        const moment_t moment = moment_from_duration(remaining_time(timer), time_min);

#if PLATFORM == PLATFORM_LINUX
        const char* format = "%02dᴴ%02dᴹ";
#else
        const char* format = "%02dh%02dm";
#endif

        drawf(
            renderer->canvas,
            vector2d_add(ctx->current, (vector2d_t) { .x = 4 }),
            (drawing_attr_t) { .color = color_white },
            format, moment.hours, moment.mins
        );

        wrap_drawing_lines(ctx, 1);
    }

    {
        static const size_t progress_bar_width = 14;
        static const size_t progress_bar_index_timeout_away_1 = (size_t)((float)progress_bar_width * 0.2f);
        static const size_t progress_bar_index_timeout_away_2 = (size_t)((float)progress_bar_width * 0.4f);

        drawing_attr_t attr = { 0 };

        const float remaining_ratio = 1 - elapsed_time_ratio(timer);

        const size_t remaining_index = (size_t)((float)progress_bar_width * remaining_ratio);
        if (remaining_index <= progress_bar_index_timeout_away_1) {
            attr.color = color_red;
        } else if (remaining_index <= progress_bar_index_timeout_away_2) {
            attr.color = color_yellow;
        } else {
            attr.color = color_green;
        }

        for (size_t i = 0; i < progress_bar_width; i++) {
            const float ratio = (float)i / (float)progress_bar_width;
            const bool remaining = ratio < remaining_ratio;

            if (!ctx->decorative) {
                draw(
                    renderer->canvas,
                    vector2d_add(ctx->current, (vector2d_t) { .x = i }),
                    ctx->attr,
                    remaining ? "─" : " "
                );
                continue;
            }

            attr.dim = !remaining;

            draw(
                renderer->canvas,
                vector2d_add(ctx->current, (vector2d_t) { .x = i }),
                attr,
                "─"
            );
        }

        wrap_drawing_lines(ctx, 1);
    }
}

static const char* water_flow_state_to_str(water_flow_state_t state);

static void render_debug_info(
    renderer_t* const renderer,
    const drawing_context_t* const ctx,
    const duration_t delta,
    const tick_timer_t* const timer, const ccodoc_t* const ccodoc
)
{
    const drawing_attr_t default_attr = (drawing_attr_t) { .color = color_white };

    vector2d_t p = {
        .x = 0,
        .y = 0,
    };

    {
        drawing_attr_t attr = default_attr;
        attr.bold = true;
        draw(renderer->canvas, p, attr, "DEBUG -------");
        p = vector2d_add(p, (vector2d_t) { .y = 1 });
    }

    {
        draw(renderer->canvas, p, default_attr, "# engine");
        p = vector2d_add(p, (vector2d_t) { .y = 1 });

        drawf(
            renderer->canvas,
            p,
            default_attr,
            "fps: %d", delta.msecs != 0 ? (int)round(1000.0 / (double)delta.msecs) : 0
        );
        p = vector2d_add(p, (vector2d_t) { .y = 1 });

        drawf(
            renderer->canvas,
            p,
            default_attr,
            "decorative: %s", ctx->decorative ? "yes" : "no"
        );
        p = vector2d_add(p, (vector2d_t) { .y = 1 });
    }

    {
        draw(renderer->canvas, p, default_attr, "# timer");
        p = vector2d_add(p, (vector2d_t) { .y = 1 });

        const moment_t m = moment_from_duration(remaining_time(timer), time_msec);
        drawf(
            renderer->canvas,
            p,
            default_attr,
            "remaining: %02d:%02d:%02d:%02d", m.hours, m.mins, m.secs, m.msecs
        );
        p = vector2d_add(p, (vector2d_t) { .y = 1 });

        drawf(
            renderer->canvas,
            p,
            default_attr,
            "elapsed time ratio: %f", elapsed_time_ratio(timer)
        );
        p = vector2d_add(p, (vector2d_t) { .y = 1 });
    }

    {
        draw(renderer->canvas, p, default_attr, "# ccodoc");
        p = vector2d_add(p, (vector2d_t) { .y = 1 });

        {
            draw(renderer->canvas, p, default_attr, "## kakehi");
            p = vector2d_add(p, (vector2d_t) { .y = 1 });

            drawf(
                renderer->canvas,
                p,
                default_attr,
                "state: %s", water_flow_state_to_str(ccodoc->kakehi.state)
            );
            p = vector2d_add(p, (vector2d_t) { .y = 1 });
        }

        {
            draw(renderer->canvas, p, default_attr, "## tsutsu");
            p = vector2d_add(p, (vector2d_t) { .y = 1 });

            drawf(
                renderer->canvas,
                p,
                default_attr,
                "state: %s", water_flow_state_to_str(ccodoc->tsutsu.state)
            );
            p = vector2d_add(p, (vector2d_t) { .y = 1 });

            drawf(
                renderer->canvas,
                p,
                default_attr,
                "water_amount_ratio: %f", tsutsu_water_amount_ratio(&ccodoc->tsutsu)
            );
            p = vector2d_add(p, (vector2d_t) { .y = 1 });
        }

        {
            draw(renderer->canvas, p, default_attr, "## hachi");
            p = vector2d_add(p, (vector2d_t) { .y = 1 });

            drawf(
                renderer->canvas,
                p,
                default_attr,
                "state: %s", water_flow_state_to_str(ccodoc->hachi.state)
            );
            p = vector2d_add(p, (vector2d_t) { .y = 1 });
        }
    }
}

static const char* water_flow_state_to_str(water_flow_state_t state)
{
    switch (state) {
    case holding_water:
        return "holding";
    case releasing_water:
        return "releasing";
    }
}

static void play_sound(const char* name);

static void on_tsutsu_poured(void* const raw_renderer)
{
    const renderer_t* const renderer = raw_renderer;
    if (renderer->sound.tsutsu_poured == NULL) {
        return;
    }

    play_sound(renderer->sound.tsutsu_poured);
}

static void on_tsutsu_released_water(void* const raw_renderer)
{
    const renderer_t* const renderer = raw_renderer;
    if (renderer->sound.tsutsu_bumped == NULL) {
        return;
    }

    play_sound(renderer->sound.tsutsu_bumped);
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
