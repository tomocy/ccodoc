#include "ccodoc.h"
#include <math.h>
#include <time.h>

extern int nanosleep(const struct timespec* requested_time, struct timespec* remaining);

static void ticker_tick(ticker* ticker, const duration delta);
static void ticker_reset(ticker* ticker);

void timer_tick(timer* timer, const duration delta)
{
    ticker_tick(&timer->ticker, delta);
}

void timer_reset(timer* timer)
{
    ticker_reset(&timer->ticker);
}

bool timer_is_timeout(const timer* timer)
{
    return timer_timeout_ratio(timer) >= 1;
}

float timer_timeout_ratio(const timer* timer)
{
    assert(timer->duration.msecs != 0);
    return (float)((double)timer->ticker.elapsed.msecs / (double)timer->duration.msecs);
}

duration timer_remaining_time(const timer* timer)
{
    return (duration) {
        .msecs = timer->duration.msecs - MIN(timer->ticker.elapsed.msecs, timer->duration.msecs),
    };
}

void sleep_for(const duration duration)
{
    if (duration.msecs < 0) {
        return;
    }

    struct timespec time_spec;
    time_spec.tv_sec = (__time_t)duration.msecs / 1000;
    time_spec.tv_nsec = (__time_t)duration.msecs % 1000 * 1000000;

    int slept = -1;
    do {
        slept = nanosleep(&time_spec, &time_spec);
    } while (slept != 0);
}

moment moment_from_duration(const duration d, time_precision precision)
{
    moment moment = { 0 };
    duration current = d;

    if (precision <= time_hour) {
        double hours = (double)current.msecs / time_hour;
        if (precision == time_hour) {
            hours = ceil(hours);
        }
        moment.hours = (unsigned int)hours;

        current.msecs -= (unsigned long)moment.hours * time_hour;
    }

    if (precision <= time_min) {
        double mins = (double)current.msecs / time_min;
        if (precision == time_min) {
            mins = ceil(mins);
        }
        mins = MIN(mins, 59);

        moment.mins = (unsigned int)mins;

        current.msecs -= (unsigned long)moment.mins * time_min;
    }

    if (precision <= time_sec) {
        double secs = (double)current.msecs / time_sec;
        if (precision == time_sec) {
            secs = ceil(secs);
        }
        secs = MIN(secs, 59);

        moment.secs = (unsigned int)secs;

        current.msecs -= (unsigned long)moment.secs * time_sec;
    }

    if (precision <= time_msec) {
        double msecs = (double)current.msecs / time_msec;
        if (precision == time_msec) {
            msecs = ceil(msecs);
        }
        msecs = MIN(msecs, 999);

        moment.msecs = (unsigned int)msecs;

        current.msecs -= (unsigned long)moment.msecs * time_msec;
    }

    return moment;
}

duration duration_from_moment(const moment moment)
{
    return (duration) {
        .msecs = moment.hours * time_hour + moment.mins * time_min + moment.secs * time_sec + moment.msecs * time_msec,
    };
}

static void ticker_tick(ticker* ticker, const duration delta)
{
    ticker->elapsed.msecs += delta.msecs;
}

static void ticker_reset(ticker* ticker)
{
    ticker->elapsed.msecs = 0;
}

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
