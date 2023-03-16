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
static void ccodoc_render_debug_info(ccodoc_rendering_context* ctx, const ccodoc* ccodoc);

static ccodoc_rendering_context ccodoc_init_rendering_context(const ccodoc_context* ctx, point origin);
static void ccodoc_wrap_rendering_lines(ccodoc_rendering_context* ctx, unsigned int n);

void ccodoc_init_renderer(ccodoc_renderer* render)
{
    setlocale(LC_ALL, "");

    render->window = initscr();
    noecho();
    curs_set(0);
}

void ccodoc_deinit_renderer(ccodoc_renderer* render)
{
    endwin();
    render->window = NULL;
}

void ccodoc_render(ccodoc_renderer* render, const ccodoc_context* ctx, const ccodoc* ccodoc)
{
    (void)render;

    ccodoc_rendering_context rctx = ccodoc_init_rendering_context(ctx, (point) { .x = 0, .y = 0 });

    clear();

    ccodoc_render_kakehi(&rctx, &ccodoc->kakehi);
    ccodoc_render_tsutsu(&rctx, &ccodoc->tsutsu);
    mvprintw(rctx.current.y, rctx.current.x, "▭▭▭▭━━━━━━▨▨▨▨");

    if (ctx->debug) {
        ccodoc_render_debug_info(&rctx, ccodoc);
    }

    refresh();
}

static void ccodoc_render_kakehi(ccodoc_rendering_context* ctx, const ccodoc_kakehi* kakehi)
{
    static const int kakehi_len = 3;

    float holding_ratio = timer_timeout_ratio(&kakehi->holding_timer);

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
        char* water = (j == holding_index) ? "━" : "═";
        mvprintw(ctx->current.y, ctx->current.x + j, water);
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

    float holding_ratio = ccodoc_tsutsu_holding_ratio(tsutsu);

    switch (tsutsu->state) {
    case ccodoc_tsutsu_state_holding:
        if (holding_ratio < 0.8) {
            tsutsu_art = tsutsu_art_jo;
        } else if (holding_ratio < 1) {
            tsutsu_art = tsutsu_art_ha;
        } else {
            tsutsu_art = tsutsu_art_kyu;
        }
        break;
    case ccodoc_tsutsu_state_releasing:
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
        mvprintw(ctx->current.y + h, ctx->current.x + 3, tsutsu_art[h]);
    }
    ccodoc_wrap_rendering_lines(ctx, tsutsu_height);
}

void ccodoc_render_debug_info(ccodoc_rendering_context* ctx, const ccodoc* ccodoc)
{
    point origin = {
        .x = 0,
        .y = ctx->current.y + 5,
    };

    mvprintw(origin.y, origin.x, "DEBUG -------");
    mvprintw(origin.y + 1, origin.x, "- tsutsu");
    mvprintw(origin.y + 2, origin.x, "holding_ratio: %f\n", ccodoc_tsutsu_holding_ratio(&ccodoc->tsutsu));
}

ccodoc_rendering_context ccodoc_init_rendering_context(const ccodoc_context* ctx, point origin)
{
    ccodoc_rendering_context rctx = {
        .app = ctx,
        .origin = origin,
        .current = origin,
    };

    return rctx;
}

void ccodoc_wrap_rendering_lines(ccodoc_rendering_context* ctx, unsigned int n)
{
    ctx->current.y += n;
    ctx->current.x = ctx->origin.x;
}
