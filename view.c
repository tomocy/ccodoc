#include "ccodoc.h"
#include <stdio.h>

static void ccodoc_kakehi_render(const ccodoc_kakehi* kakehi);
static void ccodoc_tsutsu_render(const ccodoc_tsutsu* tsutsu);

void ccodoc_render(const ccodoc_context* ctx, const ccodoc* ccodoc)
{
    system("clear");
    ccodoc_kakehi_render(&ccodoc->kakehi);
    ccodoc_tsutsu_render(&ccodoc->tsutsu);
    printf("▭▭▭▭━━━━━━▨▨▨▨\n");

    if (ctx->debug) {
        printf("\n");
        printf("tsutsu -------\n");
        printf("holding_ratio: %f\n", ccodoc_tsutsu_holding_ratio(&ccodoc->tsutsu));
    }
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
        printf("%s", water);
    }
    printf("\n");
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
        printf("   %s\n", tsutsu_art[h]);
    }
}
