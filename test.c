#include "ccodoc_test.h"

#pragma clang diagnostic ignored "-Wempty-translation-unit"

#define EXPECT_PASS(label, actual)      \
    {                                   \
        printf(label);                  \
        if ((actual) != EXIT_SUCCESS) { \
            return EXIT_FAILURE;        \
        }                               \
    }

int main(void)
{
    EXPECT_PASS("# engine\n", test_engine());
    printf("\n");

    EXPECT_PASS("# string\n", test_str());
    printf("\n");

    EXPECT_PASS("# time\n", test_time());
    printf("\n");

    printf("ALL PASS\n");

    return EXIT_SUCCESS;
}
