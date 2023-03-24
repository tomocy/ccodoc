#include "canvas.h"

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
} drawing_context;

static void draw_kakehi(drawing_context* ctx, const kakehi* kakehi);
static void draw_tsutsu(drawing_context* ctx, const tsutsu* tsutsu);
static void draw_hachi(drawing_context* ctx, const hachi* hachi);
static void draw_roji(const drawing_context* ctx);

#if !TESTING
static void on_tsutsu_poured(void);
static void on_tsutsu_released_water(void);
#endif

static void draw_timer(drawing_context* ctx, const timer* timer);

static void draw_debug_info(
    const context* ctx,
    const timer* timer,
    const ccodoc* ccodoc
);

static point drawing_window_size(const canvas* canvas);

static drawing_context init_drawing_context(const context* ctx, point origin);
static void wrap_drawing_lines(drawing_context* ctx, unsigned int n);

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
} drawing_attr;

#define WITH_DRAWING_ATTR(attr, ...)            \
    {                                           \
        const drawing_attr x = (attr);          \
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

#define PREFER_DRAWING_WITH_ATTR(with_attr, attr, ...) \
    {                                                  \
        if (with_attr) {                               \
            WITH_DRAWING_ATTR((attr), __VA_ARGS__);    \
        } else {                                       \
            __VA_ARGS__                                \
        }                                              \
    }

#if TESTING
static canvas* test_canvas = NULL;

void draw(int y, int x, const char* s)
{
    size_t i = 0;
    const char* c = s;

    while (*c) {
        char_descriptor desc = decode_char_utf8(c);
        size_t p = y * TEST_WINDOW_WIDTH + x + i;
        test_canvas->test_window[p] = desc.code;

        i++;
        c += desc.len;
    }
}
#else
#define draw(y, x, s) mvprintw((int)(y), (int)(x), (s))
#endif

#define drawf(y, x, format, ...) mvprintw((int)(y), (int)(x), (format), __VA_ARGS__)

#if !TESTING
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
#endif

void init_canvas(canvas* canvas, const context* ctx, ccodoc* ccodoc)
{
#if TESTING
    (void)canvas;
    (void)ctx;
    (void)ccodoc;

    test_canvas = canvas;
#else
    (void)setlocale(LC_ALL, "");

    canvas->window = initscr();
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
#endif
}

void deinit_canvas(canvas* canvas, ccodoc* ccodoc)
{
#if TESTING
    (void)canvas;
    (void)ccodoc;

    test_canvas = NULL;
#else
    endwin();
    canvas->window = NULL;

    ccodoc->tsutsu.on_poured = NULL;
#endif
}

void clear_canvas(canvas* canvas)
{
#if TESTING
    for (int h = 0; h < TEST_WINDOW_HEIGHT; h++) {
        for (int w = 0; w < TEST_WINDOW_WIDTH; w++) {
            canvas->test_window[h * TEST_WINDOW_WIDTH + w] = ' ';
        }
    }
#else
    (void)canvas;
    clear();
#endif
}

static void flush_canvas(canvas* canvas)
{
#if TESTING
    (void)canvas;
#else
    (void)canvas;
    refresh();
#endif
}

void draw_ccodoc(canvas* canvas, const context* ctx, const timer* timer, const ccodoc* ccodoc)
{
    static const point ccodoc_size = {
        .x = 14,
        .y = 6,
    };

    const point window_size = drawing_window_size(canvas);

    drawing_context dctx = init_drawing_context(
        ctx,
        (point) {
            .x = (window_size.x - ccodoc_size.x) / 2,
            .y = (window_size.y - ccodoc_size.y) / 2,
        }
    );

    clear_canvas(canvas);

    draw_kakehi(&dctx, &ccodoc->kakehi);
    draw_tsutsu(&dctx, &ccodoc->tsutsu);
    draw_hachi(&dctx, &ccodoc->hachi);
    draw_roji(&dctx);
    wrap_drawing_lines(&dctx, 1);

    draw_timer(&dctx, timer);

    if (ctx->debug) {
        draw_debug_info(ctx, timer, ccodoc);
    }

    flush_canvas(canvas);
}

static void draw_kakehi(drawing_context* ctx, const kakehi* kakehi)
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
            const char_descriptor desc = decode_char_utf8(c);
            const bool has_water = str_equals_n(c, "━", desc.len);

            WITH_DRAWING_ATTR(
                ((drawing_attr) {
                    .color = has_water ? color_blue : color_yellow,
                }),
                {
                    drawf(ctx->current.y, ctx->current.x + i, "%.*s", desc.len, c);
                }
            );

            i++;
            c += desc.len;
        }
    } else {
        draw(ctx->current.y, ctx->current.x, art);
    }

    wrap_drawing_lines(ctx, 1);
}

static void draw_tsutsu(drawing_context* ctx, const tsutsu* tsutsu)
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
            draw(origin.y + h, origin.x, art[h]);
            continue;
        }

        int i = 0;
        const char* c = art[h];
        while (*c) {
            const char_descriptor desc = decode_char_utf8(c);

            drawing_attr attr = (drawing_attr) { .color = color_white };

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

            WITH_DRAWING_ATTR(attr, {
                drawf(origin.y + h, origin.x + i, "%.*s", desc.len, c);
            });

            i++;
            c += desc.len;
        }
    }

    wrap_drawing_lines(ctx, art_height);
}

static void draw_hachi(drawing_context* ctx, const hachi* hachi)
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

    if (ctx->app->decorative) {
        int i = 0;
        const char* c = art;
        while (*c) {
            const char_descriptor desc = decode_char_utf8(c);
            const bool has_water = str_equals_n(c, "▬", desc.len);

            WITH_DRAWING_ATTR(
                ((drawing_attr) {
                    .color = has_water ? color_blue : color_grey,
                }),
                {
                    drawf(ctx->current.y, ctx->current.x + i, "%.*s", desc.len, c);
                }
            );

            i++;
            c += desc.len;
        }
    } else {
        draw(ctx->current.y, ctx->current.x, art);
    }

    ctx->current.x += width;
}

static void draw_roji(const drawing_context* ctx)
{
    PREFER_DRAWING_WITH_ATTR(
        ctx->app->decorative,
        ((drawing_attr) { .color = color_green, .dim = true }),
        {
            draw(ctx->current.y, ctx->current.x, "━━━━━━");
        }
    );

    PREFER_DRAWING_WITH_ATTR(
        ctx->app->decorative,
        ((drawing_attr) { .color = color_grey }),
        {
            draw(ctx->current.y, ctx->current.x + 6, "▨▨▨▨");
        }
    );
}

#if !TESTING
static void on_tsutsu_poured(void)
{
}
#endif

#if !TESTING
static void on_tsutsu_released_water(void)
{
}
#endif

static void draw_timer(drawing_context* ctx, const timer* timer)
{
    ctx->current.y += 4;

    {
        const moment moment = moment_from_duration(remaining_time(timer), time_min);

#if PLATFORM == PLATFORM_LINUX
        const char* format = "%02dᴴ%02dᴹ";
#else
        const char* format = "%02dh%02dm";
#endif

        PREFER_DRAWING_WITH_ATTR(
            ctx->app->decorative,
            ((drawing_attr) { .color = color_white }),
            {
                drawf(ctx->current.y, ctx->current.x + 4, format, moment.hours, moment.mins);
            }
        );

        wrap_drawing_lines(ctx, 1);
    }

    {
        static const size_t progress_bar_width = 14;
        static const size_t progress_bar_index_timeout_away_1 = (size_t)((float)progress_bar_width * 0.2f);
        static const size_t progress_bar_index_timeout_away_2 = (size_t)((float)progress_bar_width * 0.4f);

        drawing_attr attr = { 0 };

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
                draw(ctx->current.y, ctx->current.x + i, remaining ? "─" : " ");
                continue;
            }

            attr.dim = !remaining;

            WITH_DRAWING_ATTR(attr, {
                draw(ctx->current.y, ctx->current.x + i, "─");
            });
        }

        wrap_drawing_lines(ctx, 1);
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

static void draw_debug_info(
    const context* ctx,
    const timer* timer,
    const ccodoc* ccodoc
)
{
    PREFER_DRAWING_WITH_ATTR(
        ctx->decorative,
        ((drawing_attr) { .color = color_white }),
        {
            point p = {
                .x = 0,
                .y = 0,
            };

            PREFER_DRAWING_WITH_ATTR(
                ctx->decorative,
                ((drawing_attr) { .bold = true }),
                {
                    draw(p.y++, p.x, "DEBUG -------");
                }
            );

            {
                draw(p.y++, p.x, "# engine");

                drawf(p.y++, p.x, "decorative: %s", ctx->decorative ? "yes" : "no");
            }

            {
                draw(p.y++, p.x, "# timer");

                const moment m = moment_from_duration(remaining_time(timer), time_msec);
                drawf(p.y++, p.x, "remaining: %02d:%02d:%02d:%02d", m.hours, m.mins, m.secs, m.msecs);

                drawf(p.y++, p.x, "elapsed time ratio: %f", elapsed_time_ratio(timer));
            }

            {
                draw(p.y++, p.x, "# ccodoc");

                {
                    draw(p.y++, p.x, "## kakehi");

                    drawf(p.y++, p.x, "state: %s", water_flow_state_to_str(ccodoc->kakehi.state));
                }

                {
                    draw(p.y++, p.x, "## tsutsu");

                    drawf(p.y++, p.x, "state: %s", water_flow_state_to_str(ccodoc->tsutsu.state));
                    drawf(p.y++, p.x, "water_amount_ratio: %f", tsutsu_water_amount_ratio(&ccodoc->tsutsu));
                }

                {
                    draw(p.y++, p.x, "## hachi");

                    drawf(p.y++, p.x, "state: %s", water_flow_state_to_str(ccodoc->hachi.state));
                }
            }
        }
    );
}

static point drawing_window_size(const canvas* canvas)
{
#if TESTING
    (void)canvas;
    return (point) {
        .x = TEST_WINDOW_WIDTH,
        .y = TEST_WINDOW_HEIGHT,
    };
#else
    return (point) {
        .x = getmaxx(canvas->window),
        .y = getmaxy(canvas->window),
    };
#endif
}

static drawing_context init_drawing_context(const context* ctx, point origin)
{
    return (drawing_context) {
        .app = ctx,
        .origin = origin,
        .current = origin,
    };
}

static void wrap_drawing_lines(drawing_context* ctx, unsigned int n)
{
    ctx->current.y += n;
    ctx->current.x = ctx->origin.x;
}
