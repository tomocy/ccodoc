#include "string.h"

#include "ccodoc_test.h"
#include <string.h>

int test_str(void)
{
    EXPECT_TRUE(str_equals("foo", "foo"));

    EXPECT_FALSE(str_equals("foo", "bar"));

    EXPECT_TRUE(str_equals("", ""));

    EXPECT_TRUE(str_equals_n("foobar", "foorab", 1));
    EXPECT_TRUE(str_equals_n("foobar", "foorab", 3));
    EXPECT_FALSE(str_equals_n("foobar", "foorab", 4));
    EXPECT_FALSE(str_equals_n("foobar", "foorab", 6));

    {
        static const char* data = "あきすての🍡";

        const char* current = data;

        {
            size_t n = decode_char_utf8(current);
            EXPECT_TRUE(str_equals_n(current, "あ", n));
            EXPECT_FALSE(str_equals_n(current, "い", n));
            current += n;
        }

        {
            size_t n = decode_char_utf8(current);
            EXPECT_TRUE(str_equals_n(current, "き", n));
            EXPECT_FALSE(str_equals_n(current, "く", n));
            current += n;
        }

        {
            size_t n = decode_char_utf8(current);
            EXPECT_TRUE(str_equals_n(current, "す", n));
            EXPECT_FALSE(str_equals_n(current, "せ", n));
            current += n;
        }

        {
            size_t n = decode_char_utf8(current);
            EXPECT_TRUE(str_equals_n(current, "て", n));
            EXPECT_FALSE(str_equals_n(current, "と", n));
            current += n;
        }

        {
            size_t n = decode_char_utf8(current);
            EXPECT_TRUE(str_equals_n(current, "の", n));
            EXPECT_FALSE(str_equals_n(current, "な", n));
            current += n;
        }

        {
            size_t n = decode_char_utf8(current);
            EXPECT_TRUE(str_equals_n(current, "🍡", n));
            EXPECT_FALSE(str_equals_n(current, "🍵", n));
            current += n;
        }
    }

    return EXIT_SUCCESS;
}
