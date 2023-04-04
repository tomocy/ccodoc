#pragma once

#include <stdio.h>
#include <stdlib.h>

#define BOOL_TO_STR(x) (x) ? "true" : "false"

extern void report_status(
    const char* file, int line,
    bool passes,
    const char* label,
    const char* actual, const char* expected
);

#define REPORT_TEST_STATUS(passes, label, format, actual, expected) \
    {                                                               \
        if ((passes)) {                                             \
            printf("PASS: %s == " format "\n", label, (expected));  \
        } else {                                                    \
            printf("FAIL: %s\n", label);                            \
            printf("  expected: " format "\n", (expected));         \
            printf("  actual: " format "\n", (actual));             \
            printf("%s:%d\n", __FILE__, __LINE__);                  \
        }                                                           \
    }

#define EXPECT_EQUAL_X_BOOL_L(label, actual, expected)                                                    \
    {                                                                                                     \
        const bool actual_ = (actual);                                                                    \
        const bool expected_ = (expected);                                                                \
        const bool equals = actual_ == expected_;                                                         \
        REPORT_TEST_STATUS(equals, label, "%s", actual_ ? "true" : "false", expected_ ? "true" : "false") \
        if (!equals) {                                                                                    \
            return EXIT_FAILURE;                                                                          \
        }                                                                                                 \
    }

#define EXPECT_TRUE_X_L(label, actual) EXPECT_EQUAL_X_BOOL_L(label, actual, true)
#define EXPECT_TRUE_X(actual) EXPECT_TRUE_X_L(#actual, actual)
#define EXPECT_FALSE_X_L(label, actual) EXPECT_EQUAL_X_BOOL_L(label, actual, false)
#define EXPECT_FALSE_X(actual) EXPECT_FALSE_X_L(#actual, actual)

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
