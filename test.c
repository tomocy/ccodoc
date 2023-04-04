#include "test.h"

#pragma clang diagnostic ignored "-Wempty-translation-unit"

int main(void)
{
    printf("# ccodoc\n");
    EXPECT_PASS(test_ccodoc());
    printf("\n");

    printf("# string\n");
    EXPECT_PASS(test_str());
    printf("\n");

    printf("# time\n");
    EXPECT_PASS(test_time());
    printf("\n");

    printf("# platform\n");
    EXPECT_PASS(test_platform());
    printf("\n");

    printf("# renderer\n");
    EXPECT_PASS(test_renderer());
    printf("\n");

    printf("ALL PASS\n");

    return EXIT_SUCCESS;
}

void report_status(
    const char* const file, const int line,
    const bool passes,
    const char* const label,
    const char* const actual, const char* const expected
)
{
    if (passes) {
        printf("PASS: %s => %s\n", label, expected);
        return;
    }

    printf("FAIL: %s =>\n", label);
    printf("  expected: %s\n", expected);
    printf("  actual: %s\n", actual);
    printf("%s:%d\n", file, line);
}

void report_status_str(
    const char* const file, const int line,
    const bool passes,
    const char* const label,
    const char* const actual, const char* const expected
)
{
    char actual_label[1 << 5] = { 0 };
    (void)snprintf(actual_label, sizeof(actual_label), "\"%s\"", actual);

    char expected_label[1 << 5] = { 0 };
    (void)snprintf(expected_label, sizeof(expected_label), "\"%s\"", expected);

    report_status(file, line, passes, label, actual_label, expected_label);
}
