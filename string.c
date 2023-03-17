#include "ccodoc.h"
#include <string.h>

bool str_equals_to(char* str, char* other)
{
    return strcmp(str, other) == 0;
}

void test_str(void)
{
    printf("# string\n");

    EXPECT_TRUE(str_equals_to("foo", "foo"));

    EXPECT_FALSE(str_equals_to("foo", "bar"));

    EXPECT_TRUE(str_equals_to("", ""));
}
