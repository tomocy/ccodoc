#include "ccodoc.h"

#include "test.h"

struct ccodoc_state_t {
    struct kakehi_state_t {
        water_flow_state_t state;
        float holding_water_ratio;
        float releasing_water_ratio;
    } kakehi;

    struct tsutsu_state_t {
        water_flow_state_t state;
        float water_amount_ratio;
        float releasing_water_ratio;
    } tsutsu;

    struct hachi_state_t {
        water_flow_state_t state;
        float releasing_water_ratio;
    } hachi;
};

static int expect_tick_ccodoc(const char* file, int line, duration_t delta, ccodoc_t* ccodoc, struct ccodoc_state_t expected);
#define EXPECT_TICK_CCODOC(delta, ccodoc, expected) EXPECT_PASS(expect_tick_ccodoc(__FILE__, __LINE__, delta, ccodoc, expected))

int test_ccodoc(void)
{
    ccodoc_t ccodoc = {
        .kakehi = {
            .release_water_amount = 5,
            .holding_water = {
                .duration = { .msecs = 2500 },
            },
            .releasing_water = {
                .duration = { .msecs = 500 },
            },
        },
        .tsutsu = {
            .water_capacity = 10,
            .releasing_water = {
                .duration = { .msecs = 1500 },
            },
        },
        .hachi = {
            .releasing_water = {
                .duration = { .msecs = 1000 },
            },
        },
    };

    EXPECT_TICK_CCODOC(
        ((duration_t) { .msecs = 0 }),
        &ccodoc,
        ((struct ccodoc_state_t) {
            .kakehi = { .state = holding_water, .holding_water_ratio = 0, .releasing_water_ratio = 0 },
            .tsutsu = { .state = holding_water, .water_amount_ratio = 0, .releasing_water_ratio = 0 },
            .hachi = { .state = holding_water, .releasing_water_ratio = 0 },
        })
    );

    EXPECT_TICK_CCODOC(
        ((duration_t) { .msecs = 250 }),
        &ccodoc,
        ((struct ccodoc_state_t) {
            .kakehi = { .state = holding_water, .holding_water_ratio = 0.1, .releasing_water_ratio = 0 },
            .tsutsu = { .state = holding_water, .water_amount_ratio = 0, .releasing_water_ratio = 0 },
            .hachi = { .state = holding_water, .releasing_water_ratio = 0 },
        })
    );

    EXPECT_TICK_CCODOC(
        ((duration_t) { .msecs = 1750 }),
        &ccodoc,
        ((struct ccodoc_state_t) {
            .kakehi = { .state = holding_water, .holding_water_ratio = 0.8, .releasing_water_ratio = 0 },
            .tsutsu = { .state = holding_water, .water_amount_ratio = 0, .releasing_water_ratio = 0 },
            .hachi = { .state = holding_water, .releasing_water_ratio = 0 },
        })
    );

    EXPECT_TICK_CCODOC(
        ((duration_t) { .msecs = 500 }),
        &ccodoc,
        ((struct ccodoc_state_t) {
            .kakehi = { .state = releasing_water, .holding_water_ratio = 1, .releasing_water_ratio = 0 },
            .tsutsu = { .state = holding_water, .water_amount_ratio = 0.5, .releasing_water_ratio = 0 },
            .hachi = { .state = holding_water, .releasing_water_ratio = 0 },
        })
    );

    EXPECT_TICK_CCODOC(
        ((duration_t) { .msecs = 250 }),
        &ccodoc,
        ((struct ccodoc_state_t) {
            .kakehi = { .state = releasing_water, .holding_water_ratio = 1, .releasing_water_ratio = 0.5 },
            .tsutsu = { .state = holding_water, .water_amount_ratio = 0.5, .releasing_water_ratio = 0 },
            .hachi = { .state = holding_water, .releasing_water_ratio = 0 },
        })
    );

    EXPECT_TICK_CCODOC(
        ((duration_t) { .msecs = 250 }),
        &ccodoc,
        ((struct ccodoc_state_t) {
            .kakehi = { .state = holding_water, .holding_water_ratio = 0, .releasing_water_ratio = 1 },
            .tsutsu = { .state = holding_water, .water_amount_ratio = 0.5, .releasing_water_ratio = 0 },
            .hachi = { .state = holding_water, .releasing_water_ratio = 0 },
        })
    );

    EXPECT_TICK_CCODOC(
        ((duration_t) { .msecs = 2000 }),
        &ccodoc,
        ((struct ccodoc_state_t) {
            .kakehi = { .state = holding_water, .holding_water_ratio = 0.8, .releasing_water_ratio = 1 },
            .tsutsu = { .state = holding_water, .water_amount_ratio = 0.5, .releasing_water_ratio = 0 },
            .hachi = { .state = holding_water, .releasing_water_ratio = 0 },
        })
    );

    EXPECT_TICK_CCODOC(
        ((duration_t) { .msecs = 500 }),
        &ccodoc,
        ((struct ccodoc_state_t) {
            .kakehi = { .state = releasing_water, .holding_water_ratio = 1, .releasing_water_ratio = 0 },
            .tsutsu = { .state = releasing_water, .water_amount_ratio = 0, .releasing_water_ratio = 0 },
            .hachi = { .state = releasing_water, .releasing_water_ratio = 0.5 },
        })
    );

    EXPECT_TICK_CCODOC(
        ((duration_t) { .msecs = 250 }),
        &ccodoc,
        ((struct ccodoc_state_t) {
            .kakehi = { .state = releasing_water, .holding_water_ratio = 1, .releasing_water_ratio = 0.5 },
            .tsutsu = { .state = releasing_water, .water_amount_ratio = 0, .releasing_water_ratio = 1.0 / 6 },
            .hachi = { .state = releasing_water, .releasing_water_ratio = 0.75 },
        })
    );

    EXPECT_TICK_CCODOC(
        ((duration_t) { .msecs = 250 }),
        &ccodoc,
        ((struct ccodoc_state_t) {
            .kakehi = { .state = holding_water, .holding_water_ratio = 0, .releasing_water_ratio = 1 },
            .tsutsu = { .state = releasing_water, .water_amount_ratio = 0, .releasing_water_ratio = 1.0 / 3 },
            .hachi = { .state = holding_water, .releasing_water_ratio = 1 },
        })
    );

    EXPECT_TICK_CCODOC(
        ((duration_t) { .msecs = 1000 }),
        &ccodoc,
        ((struct ccodoc_state_t) {
            .kakehi = { .state = holding_water, .holding_water_ratio = 0.4, .releasing_water_ratio = 1 },
            .tsutsu = { .state = holding_water, .water_amount_ratio = 0, .releasing_water_ratio = 1 },
            .hachi = { .state = holding_water, .releasing_water_ratio = 1 },
        })
    );

    return EXIT_SUCCESS;
}

static const char* water_flow_state_to_str(water_flow_state_t state)
{
    switch (state) {
    case holding_water:
        return "holding";
    case releasing_water:
        return "releasing";
    }
}

static int expect_kakehi(const char* file, int line, const kakehi_t* kakehi, struct kakehi_state_t expected);
static int expect_tsutsu(const char* file, int line, const tsutsu_t* tsutsu, struct tsutsu_state_t expected);
static int expect_hachi(const char* file, int line, const hachi_t* hachi, struct hachi_state_t expected);

static int expect_tick_ccodoc(
    const char* const file, const int line,
    const duration_t delta,
    ccodoc_t* const ccodoc, const struct ccodoc_state_t expected
)
{
    tick_ccodoc(ccodoc, delta);

    printf("- tick: %ld msecs =>\n", delta.msecs);
    EXPECT_PASS(expect_kakehi(file, line, &ccodoc->kakehi, expected.kakehi));
    EXPECT_PASS(expect_tsutsu(file, line, &ccodoc->tsutsu, expected.tsutsu));
    EXPECT_PASS(expect_hachi(file, line, &ccodoc->hachi, expected.hachi));

    return EXIT_SUCCESS;
}

static int expect_kakehi(const char* const file, const int line, const kakehi_t* const kakehi, const struct kakehi_state_t expected)
{
    const struct kakehi_state_t actual = {
        .state = kakehi->state,
        .holding_water_ratio = get_action_progress_ratio(&kakehi->holding_water),
        .releasing_water_ratio = get_action_progress_ratio(&kakehi->releasing_water),
    };

    char actual_label[1 << 8] = { 0 };
    (void)snprintf(
        actual_label, sizeof(actual_label),
        "state: %s, holding_water_ratio: %f, releasing_water_ratio: %f",
        water_flow_state_to_str(actual.state), actual.holding_water_ratio, actual.releasing_water_ratio
    );

    char expected_label[1 << 8] = { 0 };
    (void)snprintf(
        expected_label, sizeof(expected_label),
        "state: %s, holding_water_ratio: %f, releasing_water_ratio: %f",
        water_flow_state_to_str(expected.state), expected.holding_water_ratio, expected.releasing_water_ratio
    );

    const bool passes = actual.state == expected.state
        && actual.holding_water_ratio == expected.holding_water_ratio
        && actual.releasing_water_ratio == expected.releasing_water_ratio;

    report_status(file, line, passes, "kakehi", actual_label, expected_label);

    return passes ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int expect_tsutsu(const char* const file, const int line, const tsutsu_t* const tsutsu, const struct tsutsu_state_t expected)
{
    const struct tsutsu_state_t actual = {
        .state = tsutsu->state,
        .water_amount_ratio = get_tsutsu_water_amount_ratio(tsutsu),
        .releasing_water_ratio = get_action_progress_ratio(&tsutsu->releasing_water),
    };

    char actual_label[1 << 8] = { 0 };
    (void)snprintf(
        actual_label, sizeof(actual_label),
        "state: %s, water_amount_ratio: %f, releasing_water_ratio: %f",
        water_flow_state_to_str(actual.state), actual.water_amount_ratio, actual.releasing_water_ratio
    );

    char expected_label[1 << 8] = { 0 };
    (void)snprintf(
        expected_label, sizeof(expected_label),
        "state: %s, water_amount_ratio: %f, releasing_water_ratio: %f",
        water_flow_state_to_str(expected.state), expected.water_amount_ratio, expected.releasing_water_ratio
    );

    const bool passes = actual.state == expected.state
        && actual.water_amount_ratio == expected.water_amount_ratio
        && actual.releasing_water_ratio == expected.releasing_water_ratio;

    report_status(file, line, passes, "tsutsu", actual_label, expected_label);

    return passes ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int expect_hachi(const char* const file, const int line, const hachi_t* const hachi, const struct hachi_state_t expected)
{
    const struct hachi_state_t actual = {
        .state = hachi->state,
        .releasing_water_ratio = get_action_progress_ratio(&hachi->releasing_water),
    };

    char actual_label[1 << 8] = { 0 };
    (void)snprintf(
        actual_label, sizeof(actual_label),
        "state: %s, releasing_water_ratio: %f",
        water_flow_state_to_str(actual.state), actual.releasing_water_ratio
    );

    char expected_label[1 << 8] = { 0 };
    (void)snprintf(
        expected_label, sizeof(expected_label),
        "state: %s, releasing_water_ratio: %f",
        water_flow_state_to_str(expected.state), expected.releasing_water_ratio
    );

    const bool passes = actual.state == expected.state
        && actual.releasing_water_ratio == expected.releasing_water_ratio;

    report_status(file, line, passes, "hachi", actual_label, expected_label);

    return passes ? EXIT_SUCCESS : EXIT_FAILURE;
}
