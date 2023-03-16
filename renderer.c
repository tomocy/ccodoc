#include "ccodoc.h"
#include <locale.h>
#include <ncursesw/curses.h>
#include <stdio.h>

static void ccodoc_kakehi_render(const ccodoc_kakehi* kakehi);
static void ccodoc_tsutsu_render(const ccodoc_tsutsu* tsutsu);

void ccodoc_renderer_init(void)
{
    setlocale(LC_ALL, "");

    initscr();
    noecho();
    curs_set(0);
}

void ccodoc_renderer_deinit(void)
{
    endwin();
}

void ccodoc_render(const ccodoc_context* ctx, const ccodoc* ccodoc)
{
    clear();

    ccodoc_kakehi_render(&ccodoc->kakehi);
    ccodoc_tsutsu_render(&ccodoc->tsutsu);
    mvprintw(5, 0, "▭▭▭▭━━━━━━▨▨▨▨");

    if (ctx->debug) {
        mvprintw(10, 0, "tsutsu -------");
        mvprintw(11, 0, "holding_ratio: %f\n", ccodoc_tsutsu_holding_ratio(&ccodoc->tsutsu));
    }

    refresh();
}

static void ccodoc_kakehi_render(const ccodoc_kakehi* kakehi)
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
        mvprintw(0, j, water);
    }
}

static void ccodoc_tsutsu_render(const ccodoc_tsutsu* tsutsu)
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
        mvprintw(1 + h, 3, tsutsu_art[h]);
    }
}
