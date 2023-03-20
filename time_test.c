#include "ccodoc.h"
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
            EXPECT(timer_timeout_ratio(&timer), 0);
            EXPECT(timer_remaining_time(&timer).msecs, 1000);
        }

        {
            printf("- tick (200ms)\n");
            timer_tick(&timer, (duration) { .msecs = 200 });
            EXPECT(timer_timeout_ratio(&timer), 0.2f);
            EXPECT(timer_remaining_time(&timer).msecs, 800);
        }

        {
            printf("- tick (400ms)\n");
            timer_tick(&timer, (duration) { .msecs = 400 });
            EXPECT(timer_timeout_ratio(&timer), 0.6f);
            EXPECT(timer_remaining_time(&timer).msecs, 400);
        }

        {
            printf("- tick (600ms)\n");
            timer_tick(&timer, (duration) { .msecs = 600 });
            EXPECT(timer_timeout_ratio(&timer), 1.2f);
            EXPECT(timer_remaining_time(&timer).msecs, 0);
        }

        {
            printf("- reset\n");
            timer_reset(&timer);
            EXPECT(timer_timeout_ratio(&timer), 0);
            EXPECT(timer_remaining_time(&timer).msecs, 1000);
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
                EXPECT(duration.msecs, 2 * time_hour + 45 * time_min + 20 * time_sec + 500 * time_msec);
            }

            {
                printf("- moment (precision: hour)\n");
                moment m = moment_from_duration(duration, time_hour);
                EXPECT(m.hours, 3);
                EXPECT(m.mins, 0);
                EXPECT(m.secs, 0);
                EXPECT(m.msecs, 0);
            }

            {
                printf("- moment (precision: min)\n");
                moment m = moment_from_duration(duration, time_min);
                EXPECT(m.hours, 2);
                EXPECT(m.mins, 46);
                EXPECT(m.secs, 0);
                EXPECT(m.msecs, 0);
            }

            {
                printf("- moment (precision: sec)\n");
                moment m = moment_from_duration(duration, time_sec);
                EXPECT(m.hours, 2);
                EXPECT(m.mins, 45);
                EXPECT(m.secs, 21);
                EXPECT(m.msecs, 0);
            }

            {
                printf("- moment (precision: msec)\n");
                moment m = moment_from_duration(duration, time_msec);
                EXPECT(m.hours, 2);
                EXPECT(m.mins, 45);
                EXPECT(m.secs, 20);
                EXPECT(m.msecs, 500);
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
                EXPECT(m.hours, 1);
                EXPECT(m.mins, 0);
                EXPECT(m.secs, 0);
                EXPECT(m.msecs, 0);
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
                EXPECT(m.hours, 1);
                EXPECT(m.mins, 0);
                EXPECT(m.secs, 0);
                EXPECT(m.msecs, 0);
            }

            {
                printf("- moment (precision: min)\n");
                moment m = moment_from_duration(duration, time_min);
                EXPECT(m.hours, 0);
                EXPECT(m.mins, 59);
                EXPECT(m.secs, 0);
                EXPECT(m.msecs, 0);
            }

            {
                printf("- moment (precision: sec)\n");
                moment m = moment_from_duration(duration, time_sec);
                EXPECT(m.hours, 0);
                EXPECT(m.mins, 59);
                EXPECT(m.secs, 59);
                EXPECT(m.msecs, 0);
            }

            {
                printf("- moment (precision: msec)\n");
                moment m = moment_from_duration(duration, time_msec);
                EXPECT(m.hours, 0);
                EXPECT(m.mins, 59);
                EXPECT(m.secs, 59);
                EXPECT(m.msecs, 999);
            }
        }
    }

    return EXIT_SUCCESS;
}
