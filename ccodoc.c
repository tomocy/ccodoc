#include "ccodoc.h"

const size_t ccodoc_art_height = 4;

// jo (序)
const char* ccodoc_art_jo[] = {
    "◥◣",
    "  ◥◣",
    "  ▕ ◥◣",
    "  ▕   ◥◣",
};

// ha (破)
const char* ccodoc_art_ha[] = {
    "",
    "◢◤◢◤◢◤◢◤",
    "  ▕ ",
    "  ▕ ",
};

// kyu (急)
const char* ccodoc_art_kyu[] = {
    "      ◢◤",
    "    ◢◤",
    "  ◢◤",
    "◢◤▕",
};

void ccodoc_pour_water(ccodoc* ccodoc, int water_amount)
{
    if (ccodoc->state != ccodoc_state_holding) {
        return;
    }

    ccodoc->water_amount += water_amount;
    if (ccodoc->water_amount < ccodoc->water_capacity) {
        return;
    }

    ccodoc->state = ccodoc_state_releasing;
    ccodoc->water_amount = ccodoc->water_capacity;
}

void ccodoc_release_water(ccodoc* ccodoc, int water_amount)
{
    if (ccodoc->state != ccodoc_state_releasing) {
        return;
    }

    ccodoc->water_amount -= water_amount;
    if (ccodoc->water_amount > 0) {
        return;
    }

    ccodoc->state = ccodoc_state_holding;
    ccodoc->water_amount = 0;
}

float ccodoc_water_ratio(const ccodoc* ccodoc)
{
    assert(ccodoc->water_capacity != 0);
    return (float)ccodoc->water_amount / ccodoc->water_capacity;
}
