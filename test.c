#include "ccodoc_test.h"

#pragma clang diagnostic ignored "-Wempty-translation-unit"

int main(void)
{
    EXPECT_PASS("# engine\n", test_engine());
    printf("\n");

    EXPECT_PASS("# string\n", test_str());
    printf("\n");

    EXPECT_PASS("# time\n", test_time());
    printf("\n");

    EXPECT_PASS("# renderer\n", test_renderer());
    printf("\n");

    printf("ALL PASS\n");

    return EXIT_SUCCESS;
}
