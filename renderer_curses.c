#include "renderer_curses.h"

#include "math.h"
#include "string.h"
#include <assert.h>
#include <curses.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct {
    unsigned int x;
    unsigned int y;
} point;

typedef struct {
    const context* app;

    point origin;
    point current;
} rendering_context;

static void render_kakehi(rendering_context* ctx, const kakehi* kakehi);
static void render_tsutsu(rendering_context* ctx, const tsutsu* tsutsu);
static void render_hachi(rendering_context* ctx, const hachi* hachi);
static void render_roji(const rendering_context* ctx);

static void on_tsutsu_poured(void);
static void on_tsutsu_released_water(void);

static void render_timer(rendering_context* ctx, const timer* timer);

static void render_debug_info(
    const context* ctx,
    const timer* timer,
    const ccodoc* ccodoc
);

static point rendering_viewport_size(const renderer_curses* renderer);

static rendering_context init_rendering_context(const context* ctx, point origin);
static void wrap_rendering_lines(rendering_context* ctx, unsigned int n);

typedef enum {
    color_black = COLOR_BLACK,
    color_red = COLOR_RED,
    color_green = COLOR_GREEN,
    color_yellow = COLOR_YELLOW,
    color_blue = COLOR_BLUE,
    color_grey = COLOR_MAGENTA,
    color_white = COLOR_WHITE,
} color;

typedef struct {
    color color;
    bool dim;
    bool bold;
    bool underline;
} rendering_attr;

#define WITH_RENDERING_ATTR(attr, ...)          \
    {                                           \
        const rendering_attr x = (attr);        \
        int attrs = 0;                          \
        attrs |= COLOR_PAIR(x.color);           \
        attrs |= x.dim ? A_DIM : 0;             \
        attrs |= x.bold ? A_BOLD : 0;           \
        attrs |= x.underline ? A_UNDERLINE : 0; \
                                                \
        attron(attrs);                          \
        __VA_ARGS__;                            \
        attroff(attrs);                         \
    }

#define PREFER_RENDERING_WITH_ATTR(with_attr, attr, ...) \
    {                                                    \
        if (with_attr) {                                 \
            WITH_RENDERING_ATTR((attr), __VA_ARGS__);    \
        } else {                                         \
            __VA_ARGS__                                  \
        }                                                \
    }

#define render(y, x, s) mvprintw((int)(y), (int)(x), (s))
#define renderf(y, x, format, ...) mvprintw((int)(y), (int)(x), (format), __VA_ARGS__)

static void set_color(color color, short r, short g, short b, short supplement)
{
    static const float factor = 1000.0f / 255;

    if (can_change_color()) {
        init_color(
            color,
            CLAMP(0, 1000, (r + 1 * supplement) * factor),
            CLAMP(0, 1000, (g + 1 * supplement) * factor),
            CLAMP(0, 1000, (b + 1 * supplement) * factor)
        );
    }

    init_pair(color, color, color_black);
}

void init_renderer(renderer_curses* renderer, const context* ctx, ccodoc* ccodoc)
{
    (void)setlocale(LC_ALL, "");

    renderer->window = initscr();
    noecho();
    curs_set(0);

    if (!ctx->decorative) {
        return;
    }

    if (has_colors()) {
        start_color();

        static const short color_supplement = 10;

        set_color(color_black, 0, 0, 0, 0);
        set_color(color_red, 255, 123, 84, color_supplement);
        set_color(color_green, 90, 190, 50, color_supplement);
        set_color(color_yellow, 205, 180, 90, color_supplement);
        set_color(color_blue, 0, 200, 220, color_supplement);
        set_color(color_grey, 170, 160, 180, color_supplement);
        set_color(color_white, 225, 230, 255, 0);

        bkgd(COLOR_PAIR(color_black));
    }

    ccodoc->tsutsu.on_poured = on_tsutsu_poured;
    ccodoc->tsutsu.on_released_water = on_tsutsu_released_water;
}

void deinit_renderer(renderer_curses* renderer, ccodoc* ccodoc)
{
    endwin();
    renderer->window = NULL;

    ccodoc->tsutsu.on_poured = NULL;
}

void render_ccodoc(renderer_curses* renderer, const context* ctx, const timer* timer, const ccodoc* ccodoc)
{
    static const point ccodoc_size = {
        .x = 12,
        .y = 8,
    };

    const point viewport_size = rendering_viewport_size(renderer);

    rendering_context rctx = init_rendering_context(
        ctx,
        (point) {
            .x = (viewport_size.x - ccodoc_size.x) / 2,
            .y = (viewport_size.y - ccodoc_size.y) / 2,
        }
    );

    clear();

    render_kakehi(&rctx, &ccodoc->kakehi);
    render_tsutsu(&rctx, &ccodoc->tsutsu);
    render_hachi(&rctx, &ccodoc->hachi);
    render_roji(&rctx);
    wrap_rendering_lines(&rctx, 1);

    render_timer(&rctx, timer);

    if (ctx->debug) {
        render_debug_info(ctx, timer, ccodoc);
    }

    refresh();
}

static void render_kakehi(rendering_context* ctx, const kakehi* kakehi)
{
    const char* art = NULL;
    switch (kakehi->state) {
    case holding_water: {
        static const float holding_ratio_1 = 1.0f / 3 * 1;
        static const float holding_ratio_2 = 1.0f / 3 * 2;

        const float holding_ratio = elapsed_time_ratio(&kakehi->holding_water_timer);

        if (0 <= holding_ratio && holding_ratio < holding_ratio_1) {
            art = "━══";
        } else if (holding_ratio_1 <= holding_ratio && holding_ratio < holding_ratio_2) {
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
            const size_t len = decode_char_utf8(c);
            const bool has_water = str_equals_n(c, "━", len);

            WITH_RENDERING_ATTR(
                ((rendering_attr) {
                    .color = has_water ? color_blue : color_yellow,
                }),
                {
                    renderf(ctx->current.y, ctx->current.x + i, "%.*s", len, c);
                }
            );

            i++;
            c += len;
        }
    } else {
        render(ctx->current.y, ctx->current.x, art);
    }

    wrap_rendering_lines(ctx, 1);
}

static void render_tsutsu(rendering_context* ctx, const tsutsu* tsutsu)
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

    point origin = ctx->current;
    origin.x += 3;

    for (size_t h = 0; h < art_height; h++) {
        if (!ctx->app->decorative) {
            render(origin.y + h, origin.x, art[h]);
            continue;
        }

        int i = 0;
        const char* c = art[h];
        while (*c) {
            const size_t len = decode_char_utf8(c);

            rendering_attr attr = (rendering_attr) { .color = color_white };

            if (
                str_equals_n(c, "◥", len)
                || str_equals_n(c, "◣", len)
                || str_equals_n(c, "◢", len)
                || str_equals_n(c, "◤", len)
            ) {
                attr.color = color_green;
            }

            if (str_equals_n(c, "▕", len)) {
                attr.color = color_yellow;
            }

            WITH_RENDERING_ATTR(attr, {
                renderf(origin.y + h, origin.x + i, "%.*s", len, c);
            });

            i++;
            c += len;
        }
    }

    wrap_rendering_lines(ctx, art_height);
}

static void render_hachi(rendering_context* ctx, const hachi* hachi)
{
    static const unsigned int width = 4;

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

    {
        int i = 0;
        const char* c = art;
        while (*c) {
            const size_t len = decode_char_utf8(c);
            const bool has_water = str_equals_n(c, "▬", len);

            WITH_RENDERING_ATTR(
                ((rendering_attr) {
                    .color = has_water ? color_blue : color_grey,
                }),
                {
                    renderf(ctx->current.y, ctx->current.x + i, "%.*s", len, c);
                }
            );

            i++;
            c += len;
        }
    }

    ctx->current.x += width;
}

static void render_roji(const rendering_context* ctx)
{
    PREFER_RENDERING_WITH_ATTR(
        ctx->app->decorative,
        ((rendering_attr) { .color = color_green, .dim = true }),
        {
            render(ctx->current.y, ctx->current.x, "━━━━━━");
        }
    );

    PREFER_RENDERING_WITH_ATTR(
        ctx->app->decorative,
        ((rendering_attr) { .color = color_grey }),
        {
            render(ctx->current.y, ctx->current.x + 6, "▨▨▨▨");
        }
    );
}

static void on_tsutsu_poured(void)
{
}

static void on_tsutsu_released_water(void)
{
}

static void render_timer(rendering_context* ctx, const timer* timer)
{
    ctx->current.y += 4;

    {
        const moment moment = moment_from_duration(remaining_time(timer), time_min);

#if PLATFORM == PLATFORM_LINUX
        const char* format = "%02dᴴ%02dᴹ";
#else
        const char* format = "%02dh%02dm";
#endif

        PREFER_RENDERING_WITH_ATTR(
            ctx->app->decorative,
            ((rendering_attr) { .color = color_white }),
            {
                renderf(ctx->current.y, ctx->current.x + 4, format, moment.hours, moment.mins);
            }
        );

        wrap_rendering_lines(ctx, 1);
    }

    {
        static const size_t progress_bar_width = 14;
        static const size_t progress_bar_index_timeout_away_1 = (size_t)((float)progress_bar_width * 0.2f);
        static const size_t progress_bar_index_timeout_away_2 = (size_t)((float)progress_bar_width * 0.4f);

        rendering_attr attr = { 0 };

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
                render(ctx->current.y, ctx->current.x + i, remaining ? "─" : " ");
                continue;
            }

            attr.dim = !remaining;

            WITH_RENDERING_ATTR(attr, {
                render(ctx->current.y, ctx->current.x + i, "─");
            });
        }

        wrap_rendering_lines(ctx, 1);
    }
}

static const char* water_flow_state_to_str(water_flow_state state)
{
    switch (state) {
    case holding_water:
        return "holding";
    case releasing_water:
        return "releasing";
    }
}

static void render_debug_info(
    const context* ctx,
    const timer* timer,
    const ccodoc* ccodoc
)
{
    PREFER_RENDERING_WITH_ATTR(
        ctx->decorative,
        ((rendering_attr) { .color = color_white }),
        {
            point p = {
                .x = 0,
                .y = 0,
            };

            PREFER_RENDERING_WITH_ATTR(
                ctx->decorative,
                ((rendering_attr) { .bold = true }),
                {
                    render(p.y++, p.x, "DEBUG -------");
                }
            );

            {
                render(p.y++, p.x, "# engine");

                renderf(p.y++, p.x, "decorative: %s", ctx->decorative ? "yes" : "no");
            }

            {
                render(p.y++, p.x, "# timer");

                const moment m = moment_from_duration(remaining_time(timer), time_msec);
                renderf(p.y++, p.x, "remaining: %02d:%02d:%02d:%02d", m.hours, m.mins, m.secs, m.msecs);

                renderf(p.y++, p.x, "elapsed time ratio: %f", elapsed_time_ratio(timer));
            }

            {
                render(p.y++, p.x, "# ccodoc");

                {
                    render(p.y++, p.x, "## kakehi");

                    renderf(p.y++, p.x, "state: %s", water_flow_state_to_str(ccodoc->kakehi.state));
                }

                {
                    render(p.y++, p.x, "## tsutsu");

                    renderf(p.y++, p.x, "state: %s", water_flow_state_to_str(ccodoc->tsutsu.state));
                    renderf(p.y++, p.x, "water_amount_ratio: %f", tsutsu_water_amount_ratio(&ccodoc->tsutsu));
                }

                {
                    render(p.y++, p.x, "## hachi");

                    renderf(p.y++, p.x, "state: %s", water_flow_state_to_str(ccodoc->hachi.state));
                }
            }
        }
    );
}

static point rendering_viewport_size(const renderer_curses* renderer)
{
    return (point) {
        .x = getmaxx(renderer->window),
        .y = getmaxy(renderer->window),
    };
}

static rendering_context init_rendering_context(const context* ctx, point origin)
{
    return (rendering_context) {
        .app = ctx,
        .origin = origin,
        .current = origin,
    };
}

static void wrap_rendering_lines(rendering_context* ctx, unsigned int n)
{
    ctx->current.y += n;
    ctx->current.x = ctx->origin.x;
}
