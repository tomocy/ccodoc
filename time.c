#include <time.h>

extern int nanosleep(const struct timespec* requested_time, struct timespec* remaining);

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
