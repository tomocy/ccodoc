#include "time.h"

#include "ccodoc_test.h"

int test_time(void)
{
    {
        printf("## timer (duration: 00:00:01:00)\n");

        timer timer = {
            .duration = { .msecs = 1000 },
        };

        {
            printf("- initial\n");
            EXPECT_EQUAL(elapsed_time_ratio(&timer), 0);
            EXPECT_EQUAL(remaining_time(&timer).msecs, 1000);
        }

        {
            printf("- tick (200ms)\n");
            tick_timer(&timer, (duration) { .msecs = 200 });
            EXPECT_EQUAL(elapsed_time_ratio(&timer), 0.2f);
            EXPECT_EQUAL(remaining_time(&timer).msecs, 800);
        }

        {
            printf("- tick (400ms)\n");
            tick_timer(&timer, (duration) { .msecs = 400 });
            EXPECT_EQUAL(elapsed_time_ratio(&timer), 0.6f);
            EXPECT_EQUAL(remaining_time(&timer).msecs, 400);
        }

        {
            printf("- tick (600ms)\n");
            tick_timer(&timer, (duration) { .msecs = 600 });
            EXPECT_EQUAL(elapsed_time_ratio(&timer), 1);
            EXPECT_EQUAL(remaining_time(&timer).msecs, 0);
        }

        {
            printf("- reset\n");
            reset_timer(&timer);
            EXPECT_EQUAL(elapsed_time_ratio(&timer), 0);
            EXPECT_EQUAL(remaining_time(&timer).msecs, 1000);
        }
    }

    {
        printf("\n## duration\n");

        {
            printf("### 02:45:20:500\n");

            duration duration = duration_from_moment((moment) {
                .hours = 2,
                .mins = 45,
                .secs = 20,
                .msecs = 500,
            });

            {
                printf("- msecs\n");
                EXPECT_EQUAL(duration.msecs, 2 * time_hour + 45 * time_min + 20 * time_sec + 500 * time_msec);
            }

            {
                printf("- moment (precision: hour)\n");
                moment m = moment_from_duration(duration, time_hour);
                EXPECT_EQUAL(m.hours, 3);
                EXPECT_EQUAL(m.mins, 0);
                EXPECT_EQUAL(m.secs, 0);
                EXPECT_EQUAL(m.msecs, 0);
            }

            {
                printf("- moment (precision: min)\n");
                moment m = moment_from_duration(duration, time_min);
                EXPECT_EQUAL(m.hours, 2);
                EXPECT_EQUAL(m.mins, 46);
                EXPECT_EQUAL(m.secs, 0);
                EXPECT_EQUAL(m.msecs, 0);
            }

            {
                printf("- moment (precision: sec)\n");
                moment m = moment_from_duration(duration, time_sec);
                EXPECT_EQUAL(m.hours, 2);
                EXPECT_EQUAL(m.mins, 45);
                EXPECT_EQUAL(m.secs, 21);
                EXPECT_EQUAL(m.msecs, 0);
            }

            {
                printf("- moment (precision: msec)\n");
                moment m = moment_from_duration(duration, time_msec);
                EXPECT_EQUAL(m.hours, 2);
                EXPECT_EQUAL(m.mins, 45);
                EXPECT_EQUAL(m.secs, 20);
                EXPECT_EQUAL(m.msecs, 500);
            }
        }

        {
            printf("\n### 01:00:00:000\n");

            duration duration = duration_from_moment((moment) {
                .hours = 1,
            });

            {
                printf("- moment (precision: min)\n");
                moment m = moment_from_duration(duration, time_min);
                EXPECT_EQUAL(m.hours, 1);
                EXPECT_EQUAL(m.mins, 0);
                EXPECT_EQUAL(m.secs, 0);
                EXPECT_EQUAL(m.msecs, 0);
            }
        }

        {
            printf("\n### 00:59:59:999\n");

            duration duration = duration_from_moment((moment) {
                .mins = 59,
                .secs = 59,
                .msecs = 999,
            });

            {
                printf("- moment (precision: hour)\n");
                moment m = moment_from_duration(duration, time_hour);
                EXPECT_EQUAL(m.hours, 1);
                EXPECT_EQUAL(m.mins, 0);
                EXPECT_EQUAL(m.secs, 0);
                EXPECT_EQUAL(m.msecs, 0);
            }

            {
                printf("- moment (precision: min)\n");
                moment m = moment_from_duration(duration, time_min);
                EXPECT_EQUAL(m.hours, 0);
                EXPECT_EQUAL(m.mins, 59);
                EXPECT_EQUAL(m.secs, 0);
                EXPECT_EQUAL(m.msecs, 0);
            }

            {
                printf("- moment (precision: sec)\n");
                moment m = moment_from_duration(duration, time_sec);
                EXPECT_EQUAL(m.hours, 0);
                EXPECT_EQUAL(m.mins, 59);
                EXPECT_EQUAL(m.secs, 59);
                EXPECT_EQUAL(m.msecs, 0);
            }

            {
                printf("- moment (precision: msec)\n");
                moment m = moment_from_duration(duration, time_msec);
                EXPECT_EQUAL(m.hours, 0);
                EXPECT_EQUAL(m.mins, 59);
                EXPECT_EQUAL(m.secs, 59);
                EXPECT_EQUAL(m.msecs, 999);
            }
        }
    }

    return EXIT_SUCCESS;
}
