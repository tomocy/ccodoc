#include <stdio.h>
#include <stdlib.h>

#define EXPECT(actual, expected)               \
    {                                          \
        const bool x = (actual) == (expected); \
        printf("%s: ", x ? "PASS" : "FAIL");   \
        printf(#actual " == " #expected);      \
        printf("\n");                          \
        if (!x) {                              \
            return EXIT_FAILURE;               \
        }                                      \
    }

#define EXPECT_TRUE(actual) EXPECT(actual, true)
#define EXPECT_FALSE(actual) EXPECT(actual, false)

extern int test_engine(void);
extern int test_str(void);
extern int test_time(void);
