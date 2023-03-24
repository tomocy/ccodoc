#include <stdio.h>
#include <stdlib.h>

#define EXPECT_EQUAL(actual, expected)         \
    {                                          \
        const bool x = (actual) == (expected); \
        printf("%s: ", x ? "PASS" : "FAIL");   \
        printf(#actual " == " #expected);      \
        printf("\n");                          \
        if (!x) {                              \
            return EXIT_FAILURE;               \
        }                                      \
    }

#define EXPECT_TRUE(actual) EXPECT_EQUAL(actual, true)
#define EXPECT_FALSE(actual) EXPECT_EQUAL(actual, false)

#define EXPECT_PASS(label, actual)      \
    {                                   \
        printf(label);                  \
        if ((actual) != EXIT_SUCCESS) { \
            return EXIT_FAILURE;        \
        }                               \
    }

extern int test_engine(void);
extern int test_str(void);
extern int test_time(void);
