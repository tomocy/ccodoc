#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern int nanosleep(const struct timespec* requested_time, struct timespec* remaining);

static const size_t ccodoc_art_height = 4;

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

void sleep_msec(int msec)
{
    if (msec < 0) {
        return;
    }

    struct timespec time_spec;
    time_spec.tv_sec = msec / 1000;
    time_spec.tv_nsec = (msec % 1000) * 1000000;

    int slept = -1;
    do {
        slept = nanosleep(&time_spec, &time_spec);
    } while (slept != 0);
}

int main()
{
    unsigned int elapsed_sec = 0;

    for (int i = 0;; i++) {
        system("clear");

        for (int j = 0; j < 3; j++) {
            char* water = (j == i % 4) ? "━" : "═";
            printf("%s", water);
        }
        printf("\n");

        const char** art = NULL;
        switch (elapsed_sec % 5) {
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

        for (size_t h = 0; h < ccodoc_art_height; h++) {
            printf("   %s\n", art[h]);
        }

        printf("▭▭▭▭━━━━━━▨▨▨▨\n");

        sleep_msec(500);

        if (i % 2 != 0) {
            continue;
        }

        elapsed_sec++;
    }

    return 0;
}
