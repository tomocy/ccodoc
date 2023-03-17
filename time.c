#include "ccodoc.h"
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
    assert(timer->duration.msec != 0);
    return (float)((double)timer->ticker.elapsed.msec / (double)timer->duration.msec);
}

void sleep_for(const duration duration)
{
    if (duration.msec < 0) {
        return;
    }

    struct timespec time_spec;
    time_spec.tv_sec = (__time_t)duration.msec / 1000;
    time_spec.tv_nsec = (__time_t)duration.msec % 1000 * 1000000;

    int slept = -1;
    do {
        slept = nanosleep(&time_spec, &time_spec);
    } while (slept != 0);
}

static void ticker_tick(ticker* ticker, const duration delta)
{
    ticker->elapsed.msec += delta.msec;
}

static void ticker_reset(ticker* ticker)
{
    ticker->elapsed.msec = 0;
}
