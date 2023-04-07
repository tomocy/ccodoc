#include "renderer.h"

#include "canvas.h"
#include "ccodoc.h"
#include "math.h"
#include "string.h"
#include <assert.h>
#include <math.h>

static void draw_canvas(renderer_t* renderer, vec2d_t point, drawing_attr_t attr, const char* s);
static void drawf_canvas(renderer_t* renderer, vec2d_t point, drawing_attr_t attr, const char* format, ...);

static void render_kakehi(renderer_t* renderer, drawing_ctx_t* ctx, const kakehi_t* kakehi);
static void render_tsutsu(renderer_t* renderer, drawing_ctx_t* ctx, const tsutsu_t* tsutsu);
static void render_hachi(renderer_t* renderer, drawing_ctx_t* ctx, const hachi_t* hachi);
static void render_roji(renderer_t* renderer, drawing_ctx_t* ctx);

void render_ccodoc(renderer_t* const renderer, drawing_ctx_t* const ctx, const ccodoc_t* ccodoc)
{
    render_kakehi(renderer, ctx, &ccodoc->kakehi);

    ctx->current = vec2d_add(ctx->current, (vec2d_t) { .x = 3 });
    render_tsutsu(renderer, ctx, &ccodoc->tsutsu);

    render_hachi(renderer, ctx, &ccodoc->hachi);

    render_roji(renderer, ctx);
}

static void render_kakehi(renderer_t* const renderer, drawing_ctx_t* const ctx, const kakehi_t* const kakehi)
{
    const char* art = NULL;
    switch (kakehi->state) {
    case holding_water: {
        static const float holding_ratio_1 = 1.0f / 3 * 1;
        static const float holding_ratio_2 = 1.0f / 3 * 2;

        const float ratio = get_action_progress_ratio(&kakehi->holding_water);

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

    {
        int i = 0;
        for (const char* c = art; *c;) {
            const char_descriptor_t desc = decode_char_utf8(c);
            const bool has_water = str_equals_n(c, "━", desc.len);

            drawf_canvas(
                renderer,
                vec2d_add(ctx->current, (vec2d_t) { .x = i }),
                (drawing_attr_t) {
                    .color = has_water ? color_blue : color_yellow,
                },
                "%.*s", desc.len, c
            );

            i++;
            c += desc.len;
        }
    }

    wrap_drawing_lines(ctx, 1);
}

static void render_tsutsu(renderer_t* const renderer, drawing_ctx_t* const ctx, const tsutsu_t* const tsutsu)
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

    const float water_amount_ratio = get_tsutsu_water_amount_ratio(tsutsu);
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
        const float ratio = get_action_progress_ratio(&tsutsu->releasing_water);

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

    for (size_t h = 0; h < art_height; h++) {
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

            drawf_canvas(
                renderer,
                vec2d_add(ctx->current, (vec2d_t) { .y = h, .x = i }),
                attr,
                "%.*s", desc.len, c
            );

            i++;
            c += desc.len;
        }
    }

    wrap_drawing_lines(ctx, art_height);
}

static void render_hachi(renderer_t* const renderer, drawing_ctx_t* const ctx, const hachi_t* const hachi)
{
    static const unsigned int art_width = 4;

    const char* art = NULL;

    switch (hachi->state) {
    case holding_water:
        art = "▭▭▭▭";
        break;
    case releasing_water: {
        float ratio = get_action_progress_ratio(&hachi->releasing_water);

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

    {
        int i = 0;
        const char* c = art;
        while (*c) {
            const char_descriptor_t desc = decode_char_utf8(c);
            const bool has_water = str_equals_n(c, "▬", desc.len);

            drawf_canvas(
                renderer,
                vec2d_add(ctx->current, (vec2d_t) { .x = i }),
                (drawing_attr_t) {
                    .color = has_water ? color_blue : color_grey,
                },
                "%.*s", desc.len, c
            );

            i++;
            c += desc.len;
        }
    }

    ctx->current = vec2d_add(ctx->current, (vec2d_t) { .x = art_width });
}

static void render_roji(renderer_t* const renderer, drawing_ctx_t* const ctx)
{
    draw_canvas(
        renderer,
        ctx->current,
        (drawing_attr_t) { .color = color_green, .dim = true },
        "━━━━━━"
    );
    ctx->current = vec2d_add(ctx->current, (vec2d_t) { .x = 6 });

    draw_canvas(
        renderer,
        ctx->current,
        (drawing_attr_t) { .color = color_grey },
        "▨▨▨▨"
    );

    wrap_drawing_lines(ctx, 1);
}

void render_timer(renderer_t* const renderer, drawing_ctx_t* const ctx, const tick_timer_t* const timer)
{
    {
        const moment_t moment = moment_from_duration(get_remaining_time(timer), time_min);

#if PLATFORM == PLATFORM_LINUX
        const char* format = "%02dᴴ%02dᴹ";
#else
        const char* format = "%02dh%02dm";
#endif

        drawf_canvas(
            renderer,
            vec2d_add(ctx->current, (vec2d_t) { .x = 4 }),
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

        const float remaining_ratio = 1 - get_elapsed_time_ratio(timer);

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

            attr.dim = !remaining;

            const char* art = "─";
            if (!renderer->ornamental) {
                art = remaining ? "─" : " ";
            }

            draw_canvas(
                renderer,
                vec2d_add(ctx->current, (vec2d_t) { .x = i }),
                attr,
                art
            );
        }

        wrap_drawing_lines(ctx, 1);
    }
}

static void render_debug_info_ccodoc(renderer_t* renderer, drawing_ctx_t* ctx, const ccodoc_t* ccodoc);
static void render_debug_info_timer(renderer_t* renderer, drawing_ctx_t* ctx, const tick_timer_t* timer);
static const char* water_flow_state_to_str(water_flow_state_t state);

void render_debug_info(
    renderer_t* const renderer,
    const duration_t delta,
    const ccodoc_t* const ccodoc,
    const tick_timer_t* const timer
)
{
    drawing_ctx_t ctx = {
        .attr = { .color = color_white },
        .origin = { .x = 0, .y = 0 },
    };
    ctx.current = ctx.origin;

    {
        drawing_attr_t attr = ctx.attr;
        attr.bold = true;
        draw_canvas(renderer, ctx.current, attr, "DEBUG -------");
        wrap_drawing_lines(&ctx, 1);
    }

    {
        draw_canvas(renderer, ctx.current, ctx.attr, "# rendering");
        wrap_drawing_lines(&ctx, 1);

        drawf_canvas(
            renderer,
            ctx.current,
            ctx.attr,
            "ornamental: %s", renderer->ornamental ? "yes" : "no"
        );
        wrap_drawing_lines(&ctx, 1);
    }

    {
        wrap_drawing_lines(&ctx, 1);
        draw_canvas(renderer, ctx.current, ctx.attr, "# process");
        wrap_drawing_lines(&ctx, 1);

        drawf_canvas(
            renderer,
            ctx.current,
            ctx.attr,
            "fps: %d", delta.msecs != 0 ? (int)round(1000.0 / (double)delta.msecs) : 0
        );
        wrap_drawing_lines(&ctx, 1);
    }

    wrap_drawing_lines(&ctx, 1);
    render_debug_info_ccodoc(renderer, &ctx, ccodoc);

    if (timer != NULL) {
        wrap_drawing_lines(&ctx, 1);
        render_debug_info_timer(renderer, &ctx, timer);
    }
}

static void render_debug_info_ccodoc(renderer_t* renderer, drawing_ctx_t* ctx, const ccodoc_t* const ccodoc)
{
    draw_canvas(renderer, ctx->current, ctx->attr, "# ccodoc");
    wrap_drawing_lines(ctx, 1);

    {
        draw_canvas(renderer, ctx->current, ctx->attr, "## kakehi");
        wrap_drawing_lines(ctx, 1);

        drawf_canvas(
            renderer,
            ctx->current,
            ctx->attr,
            "state: %s", water_flow_state_to_str(ccodoc->kakehi.state)
        );
        wrap_drawing_lines(ctx, 1);

        drawf_canvas(
            renderer,
            ctx->current,
            ctx->attr,
            "holding_water_ratio: %f", get_action_progress_ratio(&ccodoc->kakehi.holding_water)
        );
        wrap_drawing_lines(ctx, 1);

        drawf_canvas(
            renderer,
            ctx->current,
            ctx->attr,
            "releasing_water_ratio: %f", get_action_progress_ratio(&ccodoc->kakehi.releasing_water)
        );
        wrap_drawing_lines(ctx, 1);
    }

    {
        draw_canvas(renderer, ctx->current, ctx->attr, "## tsutsu");
        wrap_drawing_lines(ctx, 1);

        drawf_canvas(
            renderer,
            ctx->current,
            ctx->attr,
            "state: %s", water_flow_state_to_str(ccodoc->tsutsu.state)
        );
        wrap_drawing_lines(ctx, 1);

        drawf_canvas(
            renderer,
            ctx->current,
            ctx->attr,
            "water_amount_ratio: %f", get_tsutsu_water_amount_ratio(&ccodoc->tsutsu)
        );
        wrap_drawing_lines(ctx, 1);

        drawf_canvas(
            renderer,
            ctx->current,
            ctx->attr,
            "releasing_water_ratio: %f", get_action_progress_ratio(&ccodoc->tsutsu.releasing_water)
        );
        wrap_drawing_lines(ctx, 1);
    }

    {
        draw_canvas(renderer, ctx->current, ctx->attr, "## hachi");
        wrap_drawing_lines(ctx, 1);

        drawf_canvas(
            renderer,
            ctx->current,
            ctx->attr,
            "state: %s", water_flow_state_to_str(ccodoc->hachi.state)
        );
        wrap_drawing_lines(ctx, 1);

        drawf_canvas(
            renderer,
            ctx->current,
            ctx->attr,
            "releasing_water_ratio: %f", get_action_progress_ratio(&ccodoc->hachi.releasing_water)
        );
        wrap_drawing_lines(ctx, 1);
    }
}

static void render_debug_info_timer(renderer_t* renderer, drawing_ctx_t* ctx, const tick_timer_t* timer)
{
    draw_canvas(renderer, ctx->current, ctx->attr, "# timer");
    wrap_drawing_lines(ctx, 1);

    const moment_t m = moment_from_duration(get_remaining_time(timer), time_msec);
    drawf_canvas(
        renderer,
        ctx->current,
        ctx->attr,
        "remaining: %02d:%02d:%02d:%02d", m.hours, m.mins, m.secs, m.msecs
    );
    wrap_drawing_lines(ctx, 1);

    drawf_canvas(
        renderer,
        ctx->current,
        ctx->attr,
        "elapsed time ratio: %f", get_elapsed_time_ratio(timer)
    );
    wrap_drawing_lines(ctx, 1);
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

static void draw_canvas(renderer_t* renderer, vec2d_t point, drawing_attr_t attr, const char* s)
{
    const drawing_attr_t attr_ = renderer->ornamental ? attr : (drawing_attr_t) { 0 };
    draw(renderer->canvas, point, attr_, s);
}

static void drawf_canvas(renderer_t* renderer, vec2d_t point, drawing_attr_t attr, const char* format, ...)
{
    const drawing_attr_t attr_ = renderer->ornamental ? attr : (drawing_attr_t) { 0 };

    va_list args = { 0 };
    va_start(args, format);

    drawfv(renderer->canvas, point, attr_, format, args);

    va_end(args);
}
