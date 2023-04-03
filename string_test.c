#include "string.h"

#include "ccodoc_test.h"
#include <string.h>

int test_str(void)
{
    {
        printf("- str_equals\n");

        EXPECT_TRUE(str_equals("foo", "foo"));

        EXPECT_FALSE(str_equals("foo", "bar"));

        EXPECT_TRUE(str_equals("", ""));

        EXPECT_TRUE(str_equals_n("foobar", "foorab", 1));
        EXPECT_TRUE(str_equals_n("foobar", "foorab", 3));
        EXPECT_FALSE(str_equals_n("foobar", "foorab", 4));
        EXPECT_FALSE(str_equals_n("foobar", "foorab", 6));
    }

    {
        printf("- str_starts_with\n");

        EXPECT_TRUE(str_starts_with("foobar", "f"));
        EXPECT_FALSE(str_starts_with("foobar", "r"));
        EXPECT_TRUE(str_starts_with("foobar", "foo"));
        EXPECT_FALSE(str_equals("foobar", "bar"));
        EXPECT_TRUE(str_starts_with("foobar", "foobar"));
        EXPECT_FALSE(str_starts_with("foobar", "foobarfoo"));
    }

    {
        printf("- utf-8: encode\n");

        {
            char s[5] = { 0 };
            encode_char_utf8(s, 12356);
            EXPECT_TRUE(str_equals(s, "い"));
            EXPECT_FALSE(str_equals(s, "う"));
        }

        {
            char s[5] = { 0 };
            encode_char_utf8(s, 12377);
            EXPECT_TRUE(str_equals(s, "す"));
            EXPECT_FALSE(str_equals(s, "せ"));
        }

        {
            char s[5] = { 0 };
            encode_char_utf8(s, 12379);
            EXPECT_TRUE(str_equals(s, "せ"));
            EXPECT_FALSE(str_equals(s, "と"));
        }

        {
            char s[5] = { 0 };
            encode_char_utf8(s, 12394);
            EXPECT_TRUE(str_equals(s, "な"));
            EXPECT_FALSE(str_equals(s, "に"));
        }
    }

    {
        printf("- utf-8: decode\n");

        static const char* data = "あきすての🍡";

        const char* current = data;

        {
            const char_descriptor_t desc = decode_char_utf8(current);
            EXPECT_EQUAL(desc.code, 12354);
            EXPECT_EQUAL(desc.len, 3);
            EXPECT_TRUE(str_equals_n(current, "あ", desc.len));
            EXPECT_FALSE(str_equals_n(current, "い", desc.len));
            current += desc.len;
        }

        {
            const char_descriptor_t desc = decode_char_utf8(current);
            EXPECT_EQUAL(desc.code, 12365);
            EXPECT_EQUAL(desc.len, 3);
            EXPECT_TRUE(str_equals_n(current, "き", desc.len));
            EXPECT_FALSE(str_equals_n(current, "く", desc.len));
            current += desc.len;
        }

        {
            const char_descriptor_t desc = decode_char_utf8(current);
            EXPECT_EQUAL(desc.code, 12377);
            EXPECT_EQUAL(desc.len, 3);
            EXPECT_TRUE(str_equals_n(current, "す", desc.len));
            EXPECT_FALSE(str_equals_n(current, "せ", desc.len));
            current += desc.len;
        }

        {
            const char_descriptor_t desc = decode_char_utf8(current);
            EXPECT_EQUAL(desc.code, 12390);
            EXPECT_EQUAL(desc.len, 3);
            EXPECT_TRUE(str_equals_n(current, "て", desc.len));
            EXPECT_FALSE(str_equals_n(current, "と", desc.len));
            current += desc.len;
        }

        {
            const char_descriptor_t desc = decode_char_utf8(current);
            EXPECT_EQUAL(desc.code, 12398);
            EXPECT_EQUAL(desc.len, 3);
            EXPECT_TRUE(str_equals_n(current, "の", desc.len));
            EXPECT_FALSE(str_equals_n(current, "な", desc.len));
            current += desc.len;
        }

        {
            const char_descriptor_t desc = decode_char_utf8(current);
            EXPECT_EQUAL(desc.code, 127841);
            EXPECT_EQUAL(desc.len, 4);
            EXPECT_TRUE(str_equals_n(current, "🍡", desc.len));
            EXPECT_FALSE(str_equals_n(current, "🍵", desc.len));
            current += desc.len;
        }
    }

    return EXIT_SUCCESS;
}
