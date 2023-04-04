#include "string.h"

#include "test.h"
#include <string.h>

int test_str(void)
{
    {
        printf("## str_equals\n");

        EXPECT_TRUE_X(str_equals("", ""));
        EXPECT_TRUE_X(str_equals("foo", "foo"));
        EXPECT_FALSE_X(str_equals("foo", "bar"));

        EXPECT_TRUE_X(str_equals_n("foobar", "foorab", 1));
        EXPECT_TRUE_X(str_equals_n("foobar", "foorab", 3));
        EXPECT_FALSE_X(str_equals_n("foobar", "foorab", 4));
        EXPECT_FALSE_X(str_equals_n("foobar", "foorab", 6));
    }

    {
        printf("## str_starts_with\n");

        EXPECT_TRUE_X(str_starts_with("foobar", "f"));
        EXPECT_FALSE_X(str_starts_with("foobar", "r"));
        EXPECT_TRUE_X(str_starts_with("foobar", "foo"));
        EXPECT_TRUE_X(str_starts_with("foobar", "foobar"));
        EXPECT_FALSE_X(str_starts_with("foobar", "foobarfoo"));
    }

    {
        printf("## encode_char_utf8\n");

        {
            char s[5] = { 0 };
            encode_char_utf8(s, 0x3044);
            EXPECT_TRUE_X_L("str_equals(U+3044, \"い\")", str_equals(s, "い"));
            EXPECT_FALSE_X_L("str_equals(U+3044, \"う\")", str_equals(s, "う"));
        }

        {
            char s[5] = { 0 };
            encode_char_utf8(s, 0x3059);
            EXPECT_TRUE_X_L("str_equals(U+3059, \"す\")", str_equals(s, "す"));
            EXPECT_FALSE_X_L("str_equals(U+3059, \"せ\")", str_equals(s, "せ"));
        }

        {
            char s[5] = { 0 };
            encode_char_utf8(s, 0x3066);
            EXPECT_TRUE_X_L("str_equals(U+3066, \"て\")", str_equals(s, "て"));
            EXPECT_FALSE_X_L("str_equals(U+3066, \"と\")", str_equals(s, "と"));
        }

        {
            char s[5] = { 0 };
            encode_char_utf8(s, 0x306A);
            EXPECT_TRUE_X_L("str_equals(U+306A, \"な\")", str_equals(s, "な"));
            EXPECT_FALSE_X_L("str_equals(U+306A, \"に\")", str_equals(s, "に"));
        }
    }

    {
        printf("- decode_char_utf8\n");

        static const char* data = "あきすての🍡";

        const char* current = data;

        {
            const char_descriptor_t desc = decode_char_utf8(current);
            EXPECT_TRUE_X_L("\"あ\".code == U+3042", desc.code == 0x3042);
            EXPECT_TRUE_X_L("\"あ\".len == 3", desc.len == 3);
            current += desc.len;
        }

        {
            const char_descriptor_t desc = decode_char_utf8(current);
            EXPECT_TRUE_X_L("\"き\".code == U+304D", desc.code == 0x304D);
            EXPECT_TRUE_X_L("\"き\".len == 3", desc.len == 3);
            current += desc.len;
        }

        {
            const char_descriptor_t desc = decode_char_utf8(current);
            EXPECT_TRUE_X_L("\"す\".code == U+3059", desc.code == 0x3059);
            EXPECT_TRUE_X_L("\"す\".len == 3", desc.len == 3);
            current += desc.len;
        }

        {
            const char_descriptor_t desc = decode_char_utf8(current);
            EXPECT_TRUE_X_L("\"て\".code == U+3066", desc.code == 0x3066);
            EXPECT_TRUE_X_L("\"て\".len == 3", desc.len == 3);
            current += desc.len;
        }

        {
            const char_descriptor_t desc = decode_char_utf8(current);
            EXPECT_TRUE_X_L("\"の\".code == U+306E", desc.code == 0x306E);
            EXPECT_TRUE_X_L("\"の\".len == 3", desc.len == 3);
            current += desc.len;
        }

        {
            const char_descriptor_t desc = decode_char_utf8(current);
            EXPECT_TRUE_X_L("\"🍡\".code == U+1F361", desc.code == 0x1F361);
            EXPECT_TRUE_X_L("\"🍡\".len == 4", desc.len == 4);
            current += desc.len;
        }
    }

    return EXIT_SUCCESS;
}
