#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const int ccodoc_art_height = 4;

// jo (序)
static const char* ccodoc_art_jo[] = {
    "◥◣",
    "  ◥◣",
    "  ▕ ◥◣",
    "  ▕   ◥◣",
};

// ha (破)
static const char* ccodoc_art_ha[] = {
    "",
    "◢◤◢◤◢◤◢◤",
    "  ▕ ",
    "  ▕ ",
};

// kyu (急)
static const char* ccodoc_art_kyu[] = {
    "      ◢◤",
    "    ◢◤",
    "  ◢◤",
    "◢◤▕",
};

int main()
{
    int n = 3 * 2 - 1;

    for (int i = 0; i < n; i++) {
        system("clear");

        printf("═━═\n");

        const char** art = NULL;
        switch (i) {
        case 0:
        case 4:
            art = ccodoc_art_jo;
            break;
        case 1:
        case 3:
            art = ccodoc_art_ha;
            break;
        case 2:
            art = ccodoc_art_kyu;
            break;
        default:
            continue;
        }

        if (!art) {
            continue;
        }

        for (int h = 0; h < ccodoc_art_height; h++) {
            printf("   %s\n", art[h]);
        }

        printf("▭▭▭▭━━━━━━▨▨▨▨\n");

        if (i >= n - 1) {
            continue;
        }

        sleep(1);
    }

    return 0;
}
