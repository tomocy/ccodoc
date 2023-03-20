#include "ccodoc.h"
#include "ccodoc_test.h"

int test_str(void)
{
    EXPECT_TRUE(str_equals_to("foo", "foo"));

    EXPECT_FALSE(str_equals_to("foo", "bar"));

    EXPECT_TRUE(str_equals_to("", ""));

    return EXIT_SUCCESS;
}
