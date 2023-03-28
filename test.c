#include "ccodoc_test.h"

#pragma clang diagnostic ignored "-Wempty-translation-unit"

int main(void)
{
    printf("# engine\n");
    EXPECT_PASS(test_engine());
    printf("\n");

    printf("# string\n");
    EXPECT_PASS(test_str());
    printf("\n");

    printf("# time\n");
    EXPECT_PASS(test_time());
    printf("\n");

    printf("# renderer\n");
    EXPECT_PASS(test_renderer());
    printf("\n");

    printf("ALL PASS\n");

    return EXIT_SUCCESS;
}
