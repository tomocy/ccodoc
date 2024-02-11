#pragma once

#include <stdlib.h>

#define BOOL_TO_STR(x) (x) ? "true" : "false"

extern void report_status(
    const char* file, int line,
    bool passes,
    const char* label,
    const char* actual, const char* expected
);

extern void report_status_str(
    const char* file, int line,
    bool passes,
    const char* label,
    const char* actual, const char* expected
);

#define EXPECT_PASS(actual)             \
    {                                   \
        if ((actual) != EXIT_SUCCESS) { \
            return EXIT_FAILURE;        \
        }                               \
    }

extern int test_ccodoc(void);
extern int test_str(void);
extern int test_time(void);
extern int test_platform(void);
extern int test_renderer(void);
