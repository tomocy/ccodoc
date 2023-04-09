#include "time.h"

#include "test.h"

struct timer_state_t {
    float get_elapsed_time_ratio;
    duration_t get_remaining_time;
};
static int expect_timer(const char* file, int line, const char* label, tick_timer_t* timer, struct timer_state_t expected);
#define EXPECT_TIMER(label, timer, expected) EXPECT_PASS(expect_timer(__FILE__, __LINE__, label, timer, expected))

static int expect_moment_from_duration(const char* file, int line, duration_t duration, time_precision_t precision, moment_t expected);
#define EXPECT_MOMENT_FROM_DURATION(duration, precision, expected) EXPECT_PASS(expect_moment_from_duration(__FILE__, __LINE__, duration, precision, expected))

int test_time(void)
{
    {
        printf("## timer (duration: 00:00:01:00)\n");

        tick_timer_t timer = {
            .duration = { .msecs = 1000 },
        };

        static const struct test {
            const char* label;
            duration_t delta;
            struct timer_state_t expected;
        } tests[] = {
            (struct test) {
                .label = "initial",
                .delta = (duration_t) { .msecs = 0 },
                .expected = (struct timer_state_t) {
                    .get_elapsed_time_ratio = 0,
                    .get_remaining_time = { .msecs = 1000 },
                },
            },
            (struct test) {
                .label = "tick 200 msecs",
                .delta = (duration_t) { .msecs = 200 },
                .expected = (struct timer_state_t) {
                    .get_elapsed_time_ratio = 0.2f,
                    .get_remaining_time = { .msecs = 800 },
                },
            },
            (struct test) {
                .label = "tick 400 msecs",
                .delta = (duration_t) { .msecs = 400 },
                .expected = (struct timer_state_t) {
                    .get_elapsed_time_ratio = 0.6f,
                    .get_remaining_time = { .msecs = 400 },
                },
            },
            (struct test) {
                .label = "tick 600 msecs",
                .delta = (duration_t) { .msecs = 600 },
                .expected = (struct timer_state_t) {
                    .get_elapsed_time_ratio = 1,
                    .get_remaining_time = { .msecs = 0 },
                },
            },
        };
        static const size_t test_len = sizeof(tests) / sizeof(struct test);

        for (size_t i = 0; i < test_len; i++) {
            const struct test test = tests[i];
            tick_timer(&timer, test.delta);
            EXPECT_TIMER(test.label, &timer, test.expected);
        }

        reset_timer(&timer);
        EXPECT_TIMER(
            "reset",
            &timer,
            ((struct timer_state_t) {
                .get_elapsed_time_ratio = 0,
                .get_remaining_time = { .msecs = 1000 },
            })
        );
    }

    {
        printf("## moment from duration\n");

        {
            printf("- 02:45:20:500\n");

            const duration_t duration = duration_from_moment((moment_t) {
                .hours = 2,
                .mins = 45,
                .secs = 20,
                .msecs = 500,
            });

            EXPECT_MOMENT_FROM_DURATION(duration, time_hour, ((moment_t) { .hours = 3 }));
            EXPECT_MOMENT_FROM_DURATION(duration, time_min, ((moment_t) { .hours = 2, .mins = 46 }));
            EXPECT_MOMENT_FROM_DURATION(duration, time_sec, ((moment_t) { .hours = 2, .mins = 45, .secs = 21 }));
            EXPECT_MOMENT_FROM_DURATION(duration, time_msec, ((moment_t) { .hours = 2, .mins = 45, .secs = 20, .msecs = 500 }));
        }

        {
            printf("- 01:00:00:000\n");

            const duration_t duration = duration_from_moment((moment_t) {
                .hours = 1,
            });

            EXPECT_MOMENT_FROM_DURATION(duration, time_hour, ((moment_t) { .hours = 1 }));
            EXPECT_MOMENT_FROM_DURATION(duration, time_min, ((moment_t) { .hours = 1 }));
            EXPECT_MOMENT_FROM_DURATION(duration, time_sec, ((moment_t) { .hours = 1 }));
            EXPECT_MOMENT_FROM_DURATION(duration, time_msec, ((moment_t) { .hours = 1 }));
        }

        {
            printf("- 00:59:59:999\n");

            const duration_t duration = duration_from_moment((moment_t) {
                .mins = 59,
                .secs = 59,
                .msecs = 999,
            });

            EXPECT_MOMENT_FROM_DURATION(duration, time_hour, ((moment_t) { .hours = 1 }));
            EXPECT_MOMENT_FROM_DURATION(duration, time_min, ((moment_t) { .mins = 59 }));
            EXPECT_MOMENT_FROM_DURATION(duration, time_sec, ((moment_t) { .mins = 59, .secs = 59 }));
            EXPECT_MOMENT_FROM_DURATION(duration, time_msec, ((moment_t) { .mins = 59, .secs = 59, .msecs = 999 }));
        }
    }

    return EXIT_SUCCESS;
}

static int expect_timer(
    const char* const file, const int line,
    const char* const label,
    tick_timer_t* const timer, const struct timer_state_t expected
)
{
    const struct timer_state_t actual = {
        .get_elapsed_time_ratio = get_elapsed_time_ratio(timer),
        .get_remaining_time = get_remaining_time(timer),
    };

    char actual_label[1 << 8] = { 0 };
    (void)snprintf(
        actual_label, sizeof(actual_label),
        "get_elapsed_time_ratio: %f, get_remaining_time: %ld msecs",
        actual.get_elapsed_time_ratio, actual.get_remaining_time.msecs
    );

    char expected_label[1 << 8] = { 0 };
    (void)snprintf(
        expected_label, sizeof(expected_label),
        "get_elapsed_time_ratio: %f, get_remaining_time: %ld msecs",
        expected.get_elapsed_time_ratio, expected.get_remaining_time.msecs
    );

    const bool passes = actual.get_elapsed_time_ratio == expected.get_elapsed_time_ratio
        && actual.get_remaining_time.msecs == expected.get_remaining_time.msecs;

    report_status(file, line, passes, label, actual_label, expected_label);

    return passes ? EXIT_SUCCESS : EXIT_FAILURE;
}

static const char* time_precision_to_str(const time_precision_t precision)
{
    switch (precision) {
    case time_hour:
        return "hour";
    case time_min:
        return "min";
    case time_sec:
        return "sec";
    case time_msec:
        return "msec";
    }
}

static int expect_moment_from_duration(
    const char* const file, const int line,
    const duration_t duration, const time_precision_t precision,
    const moment_t expected
)
{
    const moment_t actual = moment_from_duration(duration, precision);

    char label[1 << 5] = { 0 };
    (void)snprintf(label, sizeof(label), "%ld msecs in %s", duration.msecs, time_precision_to_str(precision));

    char actual_label[1 << 8] = { 0 };
    (void)snprintf(
        actual_label, sizeof(actual_label),
        "hours: %d, mins: %d, secs: %d, msecs: %d",
        actual.hours, actual.mins, actual.secs, actual.msecs
    );

    char expected_label[1 << 8] = { 0 };
    (void)snprintf(
        expected_label, sizeof(expected_label),
        "hours: %d, mins: %d, secs: %d, msecs: %d",
        expected.hours, expected.mins, expected.secs, expected.msecs
    );

    const bool passes = actual.hours == expected.hours
        && actual.mins == expected.mins
        && actual.secs == expected.secs
        && actual.msecs == expected.msecs;

    report_status(file, line, passes, label, actual_label, expected_label);

    return passes ? EXIT_SUCCESS : EXIT_FAILURE;
}
