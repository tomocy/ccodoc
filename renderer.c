#include "ccodoc.h"
#include <locale.h>
#include <ncursesw/curses.h>
#include <stdio.h>

typedef struct {
    unsigned int x;
    unsigned int y;
} point;

typedef struct {
    const ccodoc_context* app;

    point origin;
    point current;
} ccodoc_rendering_context;

static void ccodoc_render_kakehi(ccodoc_rendering_context* ctx, const ccodoc_kakehi* kakehi);
static void ccodoc_render_tsutsu(ccodoc_rendering_context* ctx, const ccodoc_tsutsu* tsutsu);
static void ccodoc_render_hachi(ccodoc_rendering_context* ctx, const ccodoc_hachi* hachi);

static void ccodoc_render_timer(ccodoc_rendering_context* ctx, const timer* timer);

static void ccodoc_render_debug_info(
    const ccodoc_renderer* renderer,
    const ccodoc_context* ctx,
    const timer* timer,
    const ccodoc* ccodoc
);

static point ccodoc_get_rendering_window_size(const ccodoc_renderer* renderer);

static ccodoc_rendering_context ccodoc_init_rendering_context(const ccodoc_context* ctx, point origin);
static void ccodoc_wrap_rendering_lines(ccodoc_rendering_context* ctx, unsigned int n);

#define ccodoc_print(y, x, s) mvprintw((int)(y), (int)(x), (s))
#define ccodoc_printf(y, x, format, ...) mvprintw((int)(y), (int)(x), (format), __VA_ARGS__)

void ccodoc_init_renderer(ccodoc_renderer* render)
{
    (void)setlocale(LC_ALL, "");

    render->window = initscr();
    noecho();
    curs_set(0);
}

void ccodoc_deinit_renderer(ccodoc_renderer* render)
{
    endwin();
    render->window = NULL;
}

void ccodoc_render(ccodoc_renderer* renderer, const ccodoc_context* ctx, const timer* timer, const ccodoc* ccodoc)
{
    static const point ccodoc_size = {
        .x = 16,
        .y = 8,
    };

    const point window_size = ccodoc_get_rendering_window_size(renderer);

    ccodoc_rendering_context rctx = ccodoc_init_rendering_context(
        ctx,
        (point) {
            .x = (window_size.x - ccodoc_size.x) / 2,
            .y = (window_size.y - ccodoc_size.y) / 2,
        }
    );

    clear();

    ccodoc_render_kakehi(&rctx, &ccodoc->kakehi);
    ccodoc_render_tsutsu(&rctx, &ccodoc->tsutsu);
    ccodoc_render_hachi(&rctx, &ccodoc->hachi);
    ccodoc_print(rctx.current.y, rctx.current.x, "━━━━━━▨▨▨▨");
    ccodoc_wrap_rendering_lines(&rctx, 1);

    ccodoc_render_timer(&rctx, timer);

    if (ctx->debug) {
        ccodoc_render_debug_info(renderer, ctx, timer, ccodoc);
    }

    refresh();
}

static void ccodoc_render_kakehi(ccodoc_rendering_context* ctx, const ccodoc_kakehi* kakehi)
{
    static const int kakehi_len = 3;

    float holding_ratio = timer_timeout_ratio(&kakehi->holding_water_timer);

    int holding_index = 0;
    if (0.3 <= holding_ratio && holding_ratio < 0.6) {
        holding_index = 1;
    }
    if (0.6 <= holding_ratio && holding_ratio < 0.9) {
        holding_index = 2;
    }
    if (holding_ratio >= 0.9) {
        holding_index = 3;
    }

    for (int j = 0; j < kakehi_len; j++) {
        const char* water = (j == holding_index) ? "━" : "═";
        ccodoc_print(ctx->current.y, ctx->current.x + j, water);
    }
    ccodoc_wrap_rendering_lines(ctx, 1);
}

static void ccodoc_render_tsutsu(ccodoc_rendering_context* ctx, const ccodoc_tsutsu* tsutsu)
{
    static const size_t tsutsu_height = 4;

    // jo (序)
    static const char* tsutsu_art_jo[] = {
        "◥◣",
        "  ◥◣",
        "  ▕ ◥◣",
        "  ▕   ◥◣",
    };

    // ha (破)
    static const char* tsutsu_art_ha[] = {
        "",
        "◢◤◢◤◢◤◢◤",
        "  ▕ ",
        "  ▕ ",
    };

    // kyu (急)
    static const char* tsutsu_art_kyu[] = {
        "      ◢◤",
        "    ◢◤",
        "  ◢◤",
        "◢◤▕",
    };

    const char** tsutsu_art = NULL;

    const float holding_ratio = ccodoc_tsutsu_water_amount_ratio(tsutsu);

    switch (tsutsu->state) {
    case ccodoc_holding_water:
        if (holding_ratio < 0.8) {
            tsutsu_art = tsutsu_art_jo;
        } else if (holding_ratio < 1) {
            tsutsu_art = tsutsu_art_ha;
        } else {
            tsutsu_art = tsutsu_art_kyu;
        }
        break;
    case ccodoc_releasing_water:
        if (holding_ratio < 0.35) {
            tsutsu_art = tsutsu_art_jo;
        } else if (holding_ratio < 0.65) {
            tsutsu_art = tsutsu_art_ha;
        } else {
            tsutsu_art = tsutsu_art_kyu;
        }
        break;
    }

    assert(tsutsu != NULL);

    for (size_t h = 0; h < tsutsu_height; h++) {
        ccodoc_print(ctx->current.y + h, ctx->current.x + 3, tsutsu_art[h]);
    }
    ccodoc_wrap_rendering_lines(ctx, tsutsu_height);
}

static void ccodoc_render_hachi(ccodoc_rendering_context* ctx, const ccodoc_hachi* hachi)
{
    static unsigned int width = 4;

    switch (hachi->state) {
    case ccodoc_holding_water:
        ccodoc_print(ctx->current.y, ctx->current.x, "▭▭▭▭");
        break;
    case ccodoc_releasing_water: {
        float ratio = timer_timeout_ratio(&hachi->releasing_water_timer);

        if (ratio < 0.35) {
            ccodoc_print(ctx->current.y, ctx->current.x, "▭▬▬▭");
        } else if (ratio < 0.65) {
            ccodoc_print(ctx->current.y, ctx->current.x, "▬▭▭▬");
        } else {
            ccodoc_print(ctx->current.y, ctx->current.x, "▭▭▭▭");
        }

        break;
    }
    }

    ctx->current.x += width;
}

static const char* ccodoc_water_flow_state_str(ccodoc_water_flow_state state)
{
    switch (state) {
    case ccodoc_holding_water:
        return "holding";
    case ccodoc_releasing_water:
        return "releasing";
    }
}

static void ccodoc_render_timer(ccodoc_rendering_context* ctx, const timer* timer)
{
    const moment moment = moment_from_duration(timer_remaining_time(timer), time_min);

    ccodoc_printf(ctx->current.y + 2, ctx->current.x + 4, "%02d:%02d", moment.hours, moment.mins);
}

static void ccodoc_render_debug_info(
    const ccodoc_renderer* renderer,
    const ccodoc_context* ctx,
    const timer* timer,
    const ccodoc* ccodoc
)
{
    static const unsigned int height = 8;

    const point window_size = ccodoc_get_rendering_window_size(renderer);

    point p = {
        .x = 0,
        .y = window_size.y - height,
    };

    ccodoc_print(p.y++, p.x, "DEBUG -------");

    {
        moment m = moment_from_duration(timer_remaining_time(timer), time_msec);
        ccodoc_print(p.y++, p.x, "- timer");
        ccodoc_printf(p.y++, p.x, "remaining: %02d:%02d:%02d:%02d", m.hours, m.mins, m.secs, m.msecs);
    }

    {
        ccodoc_print(p.y++, p.x, "- engine");
        ccodoc_printf(p.y++, p.x, "fps: %d", ctx->fps);
    }

    {

        ccodoc_print(p.y++, p.x, "- kakehi");
        ccodoc_printf(p.y++, p.x, "state: %s", ccodoc_water_flow_state_str(ccodoc->kakehi.state));

        ccodoc_print(p.y++, p.x, "- tsutsu");
        ccodoc_printf(p.y++, p.x, "state: %s", ccodoc_water_flow_state_str(ccodoc->tsutsu.state));
        ccodoc_printf(p.y++, p.x, "water_amount_ratio: %f", ccodoc_tsutsu_water_amount_ratio(&ccodoc->tsutsu));

        ccodoc_print(p.y++, p.x, "- hachi");
        ccodoc_printf(p.y++, p.x, "state: %s", ccodoc_water_flow_state_str(ccodoc->hachi.state));
    }
}

static point ccodoc_get_rendering_window_size(const ccodoc_renderer* renderer)
{
    return (point) {
        .x = getmaxx(renderer->window),
        .y = getmaxy(renderer->window),
    };
}

static ccodoc_rendering_context ccodoc_init_rendering_context(const ccodoc_context* ctx, point origin)
{
    return (ccodoc_rendering_context) {
        .app = ctx,
        .origin = origin,
        .current = origin,
    };
}

static void ccodoc_wrap_rendering_lines(ccodoc_rendering_context* ctx, unsigned int n)
{
    ctx->current.y += n;
    ctx->current.x = ctx->origin.x;
}
