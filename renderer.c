#include "renderer.h"

#include "canvas.h"
#include "ccodoc.h"
#include "math.h"
#include "string.h"
#include <assert.h>
#include <math.h>

static void draw_canvas(struct renderer* renderer, struct vec2d point, struct drawing_attr attr, const char* s);
static void drawf_canvas(struct renderer* renderer, struct vec2d point, struct drawing_attr attr, const char* format, ...);

static void render_kakehi(struct renderer* renderer, struct drawing_ctx* ctx, const struct kakehi* kakehi);
static void render_tsutsu(struct renderer* renderer, struct drawing_ctx* ctx, const struct tsutsu* tsutsu);
static void render_hachi(struct renderer* renderer, struct drawing_ctx* ctx, const struct hachi* hachi);
static void render_roji(struct renderer* renderer, struct drawing_ctx* ctx);

void render_ccodoc(struct renderer* const renderer, struct drawing_ctx* const ctx, const struct ccodoc* const ccodoc)
{
    render_kakehi(renderer, ctx, &ccodoc->kakehi);

    ctx->current = vec2d_add(ctx->current, (struct vec2d) { .x = 3 });
    render_tsutsu(renderer, ctx, &ccodoc->tsutsu);

    render_hachi(renderer, ctx, &ccodoc->hachi);

    render_roji(renderer, ctx);
}

static void render_kakehi(struct renderer* const renderer, struct drawing_ctx* const ctx, const struct kakehi* const kakehi)
{
    static const char* const art_ki = u8"━══"; // ki（起）
    static const char* const art_sho = u8"═━═"; // sho（承）
    static const char* const art_ten = u8"══━"; // ten（転）
    static const char* const art_ketsu = u8"═══"; // ketsu（結）

    const char* art = NULL;

    switch (kakehi->state) {
    case holding_water: {
        static const float holding_ratio_sho = 1.0f / 3 * 1;
        static const float holding_ratio_ten = 1.0f / 3 * 2;

        const float ratio = get_action_progress_ratio(&kakehi->holding_water);

        if (0 <= ratio && ratio < holding_ratio_sho) {
            art = art_ki;
        } else if (holding_ratio_sho <= ratio && ratio < holding_ratio_ten) {
            art = art_sho;
        } else {
            art = art_ten;
        }

        break;
    }
    case releasing_water:
        art = art_ketsu;
        break;
    }

    assert(art != NULL);

    {
        int i = 0;
        for (const char* c = art; *c;) {
            const struct char_descriptor desc = decode_char_utf8(c);
            const bool has_water = str_equals_n(c, u8"━", desc.len);

            drawf_canvas(
                renderer,
                vec2d_add(ctx->current, (struct vec2d) { .x = i }),
                (struct drawing_attr) {
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

static void render_tsutsu(struct renderer* const renderer, struct drawing_ctx* const ctx, const struct tsutsu* const tsutsu)
{
    static const size_t art_height = 4;

    // jo (序)
    static const char* const art_jo[] = {
        u8"◥◣",
        u8"  ◥◣",
        u8"  ▕ ◥◣",
        u8"  ▕   ◥◣",
    };

    // ha (破)
    static const char* const art_ha[] = {
        u8"",
        u8"◢◤◢◤◢◤◢◤",
        u8"  ▕ ",
        u8"  ▕ ",
    };

    // kyu (急)
    static const char* const art_kyu[] = {
        u8"      ◢◤",
        u8"    ◢◤",
        u8"  ◢◤",
        u8"◢◤▕",
    };

    const char* const* art = NULL;

    switch (tsutsu->state) {
    case holding_water: {
        const float ratio = get_tsutsu_water_amount_ratio(tsutsu);

        if (ratio < 0.8) {
            art = art_jo;
        } else if (ratio < 1) {
            art = art_ha;
        } else {
            art = art_kyu;
        }

        break;
    }
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
            const struct char_descriptor desc = decode_char_utf8(c);

            struct drawing_attr attr = (struct drawing_attr) { .color = color_white };

            if (
                str_equals_n(c, u8"◥", desc.len)
                || str_equals_n(c, u8"◣", desc.len)
                || str_equals_n(c, u8"◢", desc.len)
                || str_equals_n(c, u8"◤", desc.len)
            ) {
                attr.color = color_green;
            }

            if (str_equals_n(c, u8"▕", desc.len)) {
                attr.color = color_yellow;
            }

            drawf_canvas(
                renderer,
                vec2d_add(ctx->current, (struct vec2d) { .y = h, .x = i }),
                attr,
                "%.*s", desc.len, c
            );

            i++;
            c += desc.len;
        }
    }

    wrap_drawing_lines(ctx, art_height);
}

static void render_hachi(struct renderer* const renderer, struct drawing_ctx* const ctx, const struct hachi* const hachi)
{
    static const unsigned int art_width = 4;

    static const char* const art_jo = u8"▭▭▭▭";
    static const char* const art_ha = u8"▭▬▬▭";
    static const char* const art_kyu = u8"▬▭▭▬";

    const char* art = NULL;

    switch (hachi->state) {
    case holding_water:
        art = art_jo;
        break;
    case releasing_water: {
        float ratio = get_action_progress_ratio(&hachi->releasing_water);

        if (ratio < 0.35) {
            art = art_ha;
        } else if (ratio < 0.65) {
            art = art_kyu;
        } else {
            art = art_jo;
        }

        break;
    }
    }

    {
        int i = 0;
        const char* c = art;
        while (*c) {
            const struct char_descriptor desc = decode_char_utf8(c);
            const bool has_water = str_equals_n(c, u8"▬", desc.len);

            drawf_canvas(
                renderer,
                vec2d_add(ctx->current, (struct vec2d) { .x = i }),
                (struct drawing_attr) {
                    .color = has_water ? color_blue : color_grey,
                },
                "%.*s", desc.len, c
            );

            i++;
            c += desc.len;
        }
    }

    ctx->current = vec2d_add(ctx->current, (struct vec2d) { .x = art_width });
}

static void render_roji(struct renderer* const renderer, struct drawing_ctx* const ctx)
{
    draw_canvas(
        renderer,
        ctx->current,
        (struct drawing_attr) { .color = color_green, .dim = true },
        "━━━━━━"
    );
    ctx->current = vec2d_add(ctx->current, (struct vec2d) { .x = 6 });

    draw_canvas(
        renderer,
        ctx->current,
        (struct drawing_attr) { .color = color_grey },
        "▨▨▨▨"
    );

    wrap_drawing_lines(ctx, 1);
}

void render_timer(struct renderer* const renderer, struct drawing_ctx* const ctx, const struct timer* const timer)
{
    {
        const struct moment moment = moment_from_duration(get_remaining_time(timer), time_min);

#if PLATFORM != PLATFORM_MACOS
        const char* const format = "%02dᴴ%02dᴹ";
#else
        // Curses available in MacOS fails to handle some multibyte characters including 'ᴴ' and 'ᴹ',
        // requiring us to use alternative ones for them.
        const char* const format = "%02dh%02dm";
#endif

        drawf_canvas(
            renderer,
            vec2d_add(ctx->current, (struct vec2d) { .x = 4 }),
            (struct drawing_attr) { .color = color_white },
            format, moment.hours, moment.mins
        );

        wrap_drawing_lines(ctx, 1);
    }

    {
        static const size_t progress_bar_width = 14;
        static const size_t progress_bar_index_timeout_away1 = (size_t)((float)progress_bar_width * 0.2f);
        static const size_t progress_bar_index_timeout_away2 = (size_t)((float)progress_bar_width * 0.4f);

        struct drawing_attr attr = { 0 };

        const float remaining_ratio = 1 - get_elapsed_time_ratio(timer);

        const size_t remaining_index = (size_t)((float)progress_bar_width * remaining_ratio);
        if (remaining_index <= progress_bar_index_timeout_away1) {
            attr.color = color_red;
        } else if (remaining_index <= progress_bar_index_timeout_away2) {
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
                vec2d_add(ctx->current, (struct vec2d) { .x = i }),
                attr,
                art
            );
        }

        wrap_drawing_lines(ctx, 1);
    }
}

static void render_debug_info_ccodoc(struct renderer* renderer, struct drawing_ctx* ctx, const struct ccodoc* ccodoc);
static void render_debug_info_timer(struct renderer* renderer, struct drawing_ctx* ctx, const struct timer* timer);
static const char* water_flow_state_to_str(enum water_flow_state state);

void render_debug_info(
    struct renderer* const renderer,
    const struct duration delta,
    const struct ccodoc* const ccodoc,
    const struct timer* const timer
)
{
    struct drawing_ctx ctx = {
        .attr = { .color = color_white },
        .origin = { .x = 0, .y = 0 },
    };
    ctx.current = ctx.origin;

    {
        struct drawing_attr attr = ctx.attr;
        attr.bold = true;
        draw_canvas(renderer, ctx.current, attr, "DEBUG -------");
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

    wrap_drawing_lines(&ctx, 1);
    render_debug_info_ccodoc(renderer, &ctx, ccodoc);

    if (timer != NULL) {
        wrap_drawing_lines(&ctx, 1);
        render_debug_info_timer(renderer, &ctx, timer);
    }
}

static void render_debug_info_ccodoc(struct renderer* const renderer, struct drawing_ctx* const ctx, const struct ccodoc* const ccodoc)
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

static void render_debug_info_timer(struct renderer* const renderer, struct drawing_ctx* const ctx, const struct timer* const timer)
{
    draw_canvas(renderer, ctx->current, ctx->attr, "# timer");
    wrap_drawing_lines(ctx, 1);

    const struct moment m = moment_from_duration(get_remaining_time(timer), time_msec);
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

static const char* water_flow_state_to_str(enum water_flow_state state)
{
    switch (state) {
    case holding_water:
        return "holding";
    case releasing_water:
        return "releasing";
    }
}

static void draw_canvas(struct renderer* const renderer, const struct vec2d point, const struct drawing_attr attr, const char* const s)
{
    const struct drawing_attr attr2 = renderer->ornamental ? attr : (struct drawing_attr) { 0 };
    draw(renderer->canvas, point, attr2, s);
}

static void drawf_canvas(struct renderer* const renderer, const struct vec2d point, const struct drawing_attr attr, const char* const format, ...)
{
    const struct drawing_attr attr2 = renderer->ornamental ? attr : (struct drawing_attr) { 0 };

    va_list args = { 0 };
    va_start(args, format);

    drawfv(renderer->canvas, point, attr2, format, args);

    va_end(args);
}
