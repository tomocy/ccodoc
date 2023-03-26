#include "renderer.h"

#include "canvas.h"
#include "string.h"
#include <assert.h>
#include <math.h>

static void on_tsutsu_poured(void);
static void on_tsutsu_released_water(void);

void init_renderer(renderer_t* renderer, canvas_t* canvas, ccodoc_t* ccodoc)
{
    renderer->canvas = canvas;

    ccodoc->tsutsu.on_poured = on_tsutsu_poured;
    ccodoc->tsutsu.on_released_water = on_tsutsu_released_water;
}

void deinit_renderer(renderer_t* renderer, ccodoc_t* ccodoc)
{
    deinit_canvas(renderer->canvas);

    ccodoc->tsutsu.on_poured = NULL;
    ccodoc->tsutsu.on_released_water = NULL;
}

static void render_kakehi(renderer_t* renderer, drawing_context_t* ctx, const kakehi_t* kakehi);
static void render_tsutsu(renderer_t* renderer, drawing_context_t* ctx, const tsutsu_t* tsutsu);
static void render_hachi(renderer_t* renderer, drawing_context_t* ctx, const hachi_t* hachi);
static void render_roji(renderer_t* renderer, drawing_context_t* ctx);
static void render_timer(renderer_t* renderer, drawing_context_t* ctx, const tick_timer_t* timer);
static void render_debug_info(renderer_t* renderer, const duration_t delta, const context_t* ctx, const tick_timer_t* timer, const ccodoc_t* ccodoc);

void render_ccodoc(renderer_t* renderer, const duration_t delta, const context_t* ctx, const tick_timer_t* timer, const ccodoc_t* ccodoc)
{
    static const point_t ccodoc_size = {
        .x = 14,
        .y = 6,
    };

    const point_t canvas_size = get_canvas_size(renderer->canvas);

    drawing_context_t dctx = init_drawing_context(
        ctx,
        (point_t) {
            .x = (canvas_size.x - ccodoc_size.x) / 2,
            .y = (canvas_size.y - ccodoc_size.y) / 2,
        }
    );

    clear_canvas(renderer->canvas);

    render_kakehi(renderer, &dctx, &ccodoc->kakehi);
    render_tsutsu(renderer, &dctx, &ccodoc->tsutsu);
    render_hachi(renderer, &dctx, &ccodoc->hachi);
    render_roji(renderer, &dctx);

    render_timer(renderer, &dctx, timer);

    if (ctx->debug) {
        render_debug_info(renderer, delta, ctx, timer, ccodoc);
    }

    flush_canvas(renderer->canvas);
}

static void on_tsutsu_poured(void) { }

static void on_tsutsu_released_water(void) { }

static void render_kakehi(renderer_t* renderer, drawing_context_t* ctx, const kakehi_t* kakehi)
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

    if (ctx->app->decorative) {
        int i = 0;
        for (const char* c = art; *c;) {
            const char_descriptor_t desc = decode_char_utf8(c);
            const bool has_water = str_equals_n(c, "━", desc.len);

            WITH_DRAWING_ATTR(
                renderer->canvas,
                ((drawing_attr_t) {
                    .color = has_water ? color_blue : color_yellow,
                }),
                {
                    drawf(renderer->canvas, ctx->current.y, ctx->current.x + i, "%.*s", desc.len, c);
                }
            );

            i++;
            c += desc.len;
        }
    } else {
        draw(renderer->canvas, ctx->current.y, ctx->current.x, art);
    }

    wrap_drawing_lines(ctx, 1);
}

static void render_tsutsu(renderer_t* renderer, drawing_context_t* ctx, const tsutsu_t* tsutsu)
{
    static const size_t art_height = 4;

    // jo (序)
    static const char* art_jo[] = {
        "◥◣",
        "  ◥◣",
        "  ▕ ◥◣",
        "  ▕   ◥◣",
    };

    // ha (破)
    static const char* art_ha[] = {
        "",
        "◢◤◢◤◢◤◢◤",
        "  ▕ ",
        "  ▕ ",
    };

    // kyu (急)
    static const char* art_kyu[] = {
        "      ◢◤",
        "    ◢◤",
        "  ◢◤",
        "◢◤▕",
    };

    const float water_amount_ratio = tsutsu_water_amount_ratio(tsutsu);
    const char** art = NULL;

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

    point_t origin = ctx->current;
    origin.x += 3;

    for (size_t h = 0; h < art_height; h++) {
        if (!ctx->app->decorative) {
            draw(renderer->canvas, origin.y + h, origin.x, art[h]);
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

            WITH_DRAWING_ATTR(renderer->canvas, attr, {
                drawf(renderer->canvas, origin.y + h, origin.x + i, "%.*s", desc.len, c);
            });

            i++;
            c += desc.len;
        }
    }

    wrap_drawing_lines(ctx, art_height);
}

static void render_hachi(renderer_t* renderer, drawing_context_t* ctx, const hachi_t* hachi)
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

    if (ctx->app->decorative) {
        int i = 0;
        const char* c = art;
        while (*c) {
            const char_descriptor_t desc = decode_char_utf8(c);
            const bool has_water = str_equals_n(c, "▬", desc.len);

            WITH_DRAWING_ATTR(
                renderer->canvas,
                ((drawing_attr_t) {
                    .color = has_water ? color_blue : color_grey,
                }),
                {
                    drawf(renderer->canvas, ctx->current.y, ctx->current.x + i, "%.*s", desc.len, c);
                }
            );

            i++;
            c += desc.len;
        }
    } else {
        draw(renderer->canvas, ctx->current.y, ctx->current.x, art);
    }

    ctx->current.x += art_width;
}

static void render_roji(renderer_t* renderer, drawing_context_t* ctx)
{
    PREFER_DRAWING_WITH_ATTR(
        ctx->app->decorative,
        renderer->canvas,
        ((drawing_attr_t) { .color = color_green, .dim = true }),
        {
            draw(renderer->canvas, ctx->current.y, ctx->current.x, "━━━━━━");
            ctx->current.x += 6;
        }
    );

    PREFER_DRAWING_WITH_ATTR(
        ctx->app->decorative,
        renderer->canvas,
        ((drawing_attr_t) { .color = color_grey }),
        {
            draw(renderer->canvas, ctx->current.y, ctx->current.x, "▨▨▨▨");
        }
    );

    wrap_drawing_lines(ctx, 1);
}

static void render_timer(renderer_t* renderer, drawing_context_t* ctx, const tick_timer_t* timer)
{
    ctx->current.y += 4;

    {
        const moment_t moment = moment_from_duration(remaining_time(timer), time_min);

#if PLATFORM == PLATFORM_LINUX
        const char* format = "%02dᴴ%02dᴹ";
#else
        const char* format = "%02dh%02dm";
#endif

        PREFER_DRAWING_WITH_ATTR(
            ctx->app->decorative,
            renderer->canvas,
            ((drawing_attr_t) { .color = color_white }),
            {
                drawf(renderer->canvas, ctx->current.y, ctx->current.x + 4, format, moment.hours, moment.mins);
            }
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

            if (!ctx->app->decorative) {
                draw(renderer->canvas, ctx->current.y, ctx->current.x + i, remaining ? "─" : " ");
                continue;
            }

            attr.dim = !remaining;

            WITH_DRAWING_ATTR(renderer->canvas, attr, {
                draw(renderer->canvas, ctx->current.y, ctx->current.x + i, "─");
            });
        }

        wrap_drawing_lines(ctx, 1);
    }
}

static const char* water_flow_state_to_str(water_flow_state_t state);

static void render_debug_info(renderer_t* renderer, const duration_t delta, const context_t* ctx, const tick_timer_t* timer, const ccodoc_t* ccodoc)
{
    PREFER_DRAWING_WITH_ATTR(
        ctx->decorative,
        renderer->canvas,
        ((drawing_attr_t) { .color = color_white }),
        {
            point_t p = {
                .x = 0,
                .y = 0,
            };

            PREFER_DRAWING_WITH_ATTR(
                ctx->decorative,
                renderer->canvas,
                ((drawing_attr_t) { .bold = true }),
                {
                    draw(renderer->canvas, p.y++, p.x, "DEBUG -------");
                }
            );

            {
                draw(renderer->canvas, p.y++, p.x, "# engine");

                drawf(
                    renderer->canvas,
                    p.y++, p.x,
                    "fps: %d", delta.msecs != 0 ? (int)round(1000.0 / delta.msecs) : 0
                );
                drawf(renderer->canvas, p.y++, p.x, "decorative: %s", ctx->decorative ? "yes" : "no");
            }

            {
                draw(renderer->canvas, p.y++, p.x, "# timer");

                const moment_t m = moment_from_duration(remaining_time(timer), time_msec);
                drawf(renderer->canvas, p.y++, p.x, "remaining: %02d:%02d:%02d:%02d", m.hours, m.mins, m.secs, m.msecs);

                drawf(renderer->canvas, p.y++, p.x, "elapsed time ratio: %f", elapsed_time_ratio(timer));
            }

            {
                draw(renderer->canvas, p.y++, p.x, "# ccodoc");

                {
                    draw(renderer->canvas, p.y++, p.x, "## kakehi");

                    drawf(renderer->canvas, p.y++, p.x, "state: %s", water_flow_state_to_str(ccodoc->kakehi.state));
                }

                {
                    draw(renderer->canvas, p.y++, p.x, "## tsutsu");

                    drawf(renderer->canvas, p.y++, p.x, "state: %s", water_flow_state_to_str(ccodoc->tsutsu.state));
                    drawf(renderer->canvas, p.y++, p.x, "water_amount_ratio: %f", tsutsu_water_amount_ratio(&ccodoc->tsutsu));
                }

                {
                    draw(renderer->canvas, p.y++, p.x, "## hachi");

                    drawf(renderer->canvas, p.y++, p.x, "state: %s", water_flow_state_to_str(ccodoc->hachi.state));
                }
            }
        }
    );
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
