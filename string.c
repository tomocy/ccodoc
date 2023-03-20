#include "ccodoc.h"
#include <string.h>

bool str_equals_to(const char* str, const char* other)
{
    return strcmp(str, other) == 0;
}

int test_str(void)
{
    EXPECT_TRUE(str_equals_to("foo", "foo"));

    EXPECT_FALSE(str_equals_to("foo", "bar"));

    EXPECT_TRUE(str_equals_to("", ""));

    return EXIT_SUCCESS;
}
