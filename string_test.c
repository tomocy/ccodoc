#include "string.h"

#include "test.h"
#include <string.h>

static int expect_str_equals(const char* file, int line, const char* str, const char* other, bool expected);
#define EXPECT_STR_EQUALS(str, other, expected) EXPECT_PASS(expect_str_equals(__FILE__, __LINE__, str, other, expected))

static int expect_str_equals_n(const char* file, int line, const char* str, const char* other, unsigned int n, bool expected);
#define EXPECT_STR_EQUALS_N(str, other, n, expected) EXPECT_PASS(expect_str_equals_n(__FILE__, __LINE__, str, other, n, expected))

static int expect_str_starts_with(const char* file, int line, const char* str, const char* prefix, bool expected);
#define EXPECT_STR_STARTS_WITH(str, prefix, expected) EXPECT_PASS(expect_str_starts_with(__FILE__, __LINE__, str, prefix, expected))

static int expect_encode_char_utf8(const char* file, int line, uint32_t code, const char* expected);
#define EXPECT_ENCODE_CHAR_UTF8(code, expected) EXPECT_PASS(expect_encode_char_utf8(__FILE__, __LINE__, code, expected))

static int expect_decode_char_utf8(const char* file, int line, const char* src, struct char_descriptor expected);
#define EXPECT_DECODE_CHAR_UTF8(src, expected) EXPECT_PASS(expect_decode_char_utf8(__FILE__, __LINE__, src, expected))

int test_str(void)
{
    {
        printf("## str_equals\n");

        static const struct test {
            const char* str;
            const char* other;
            bool expected;
        } tests[] = {
            (struct test) { .str = "", .other = "", .expected = true },
            (struct test) { .str = "foo", .other = "foo", .expected = true },
            (struct test) { .str = "foo", .other = "bar", .expected = false },
        };
        static const size_t test_len = sizeof(tests) / sizeof(struct test);

        for (size_t i = 0; i < test_len; i++) {
            const struct test test = tests[i];
            EXPECT_STR_EQUALS(test.str, test.other, test.expected);
        }
    }

    {
        printf("## str_equals_n\n");

        static const struct test {
            const char* str;
            const char* other;
            unsigned int n;
            bool expected;
        } tests[] = {
            (struct test) { .str = "foobar", .other = "foorab", .n = 1, .expected = true },
            (struct test) { .str = "foobar", .other = "foorab", .n = 3, .expected = true },
            (struct test) { .str = "foobar", .other = "foorab", .n = 4, .expected = false },
            (struct test) { .str = "foobar", .other = "foorab", .n = 6, .expected = false },
        };
        static const size_t test_len = sizeof(tests) / sizeof(struct test);

        for (size_t i = 0; i < test_len; i++) {
            const struct test test = tests[i];
            EXPECT_STR_EQUALS_N(test.str, test.other, test.n, test.expected);
        }
    }

    {
        printf("## str_starts_with\n");

        static const struct test {
            const char* str;
            const char* prefix;
            bool expected;
        } tests[] = {
            (struct test) { .str = "foobar", .prefix = "f", .expected = true },
            (struct test) { .str = "foobar", .prefix = "r", .expected = false },
            (struct test) { .str = "foobar", .prefix = "foo", .expected = true },
            (struct test) { .str = "foobar", .prefix = "foobar", .expected = true },
            (struct test) { .str = "foobar", .prefix = "foobarfoo", .expected = false },
        };
        static const size_t test_len = sizeof(tests) / sizeof(struct test);

        for (size_t i = 0; i < test_len; i++) {
            const struct test test = tests[i];
            EXPECT_STR_STARTS_WITH(test.str, test.prefix, test.expected);
        }
    }

    {
        printf("## encode_char_utf8\n");

        static const struct test {
            uint32_t code;
            const char* expected;
        } tests[] = {
            (struct test) { .code = 0x3044, .expected = "い" },
            (struct test) { .code = 0x3059, .expected = "す" },
            (struct test) { .code = 0x3066, .expected = "て" },
            (struct test) { .code = 0x306A, .expected = "な" },
        };
        static const size_t tests_len = sizeof(tests) / sizeof(struct test);

        for (size_t i = 0; i < tests_len; i++) {
            const struct test test = tests[i];
            EXPECT_ENCODE_CHAR_UTF8(test.code, test.expected);
        }
    }

    {
        printf("## decode_char_utf8\n");

        static const struct test {
            const char* src;
            struct char_descriptor expected;
        } tests[] = {
            (struct test) {
                .src = "あ",
                .expected = { .code = 0x3042, .len = 3 },
            },
            (struct test) {
                .src = "き",
                .expected = { .code = 0x304D, .len = 3 },
            },
            (struct test) {
                .src = "す",
                .expected = { .code = 0x3059, .len = 3 },
            },
            (struct test) {
                .src = "て",
                .expected = { .code = 0x3066, .len = 3 },
            },
            (struct test) {
                .src = "の",
                .expected = { .code = 0x306E, .len = 3 },
            },
            (struct test) {
                .src = "🍡",
                .expected = { .code = 0x1F361, .len = 4 },
            },
        };
        static const size_t tests_len = sizeof(tests) / sizeof(struct test);

        for (size_t i = 0; i < tests_len; i++) {
            const struct test test = tests[i];
            EXPECT_DECODE_CHAR_UTF8(test.src, test.expected);
        }
    }

    return EXIT_SUCCESS;
}

static int expect_str_equals(const char* const file, const int line, const char* const str, const char* const other, const bool expected)
{
    const bool actual = str_equals(str, other);

    char label[1 << 5] = { 0 };
    (void)snprintf(label, sizeof(label), "\"%s\" == \"%s\"", str, other);

    const bool passes = actual == expected;

    report_status(file, line, passes, label, BOOL_TO_STR(actual), BOOL_TO_STR(expected));

    return passes ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int expect_str_equals_n(const char* const file, const int line, const char* const str, const char* const other, unsigned int n, const bool expected)
{
    const bool actual = str_equals_n(str, other, n);

    char label[1 << 5] = { 0 };
    (void)snprintf(label, sizeof(label), "\"%s\" == \"%s\"[0:%d]", str, other, n);

    const bool passes = actual == expected;

    report_status(file, line, passes, label, BOOL_TO_STR(actual), BOOL_TO_STR(expected));

    return passes ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int expect_str_starts_with(const char* file, int line, const char* str, const char* prefix, bool expected)
{
    const bool actual = str_starts_with(str, prefix);

    char label[1 << 5] = { 0 };
    (void)snprintf(label, sizeof(label), "\"%s\" starts with \"%s\"", str, prefix);

    const bool passes = actual == expected;

    report_status(file, line, passes, label, BOOL_TO_STR(actual), BOOL_TO_STR(expected));

    return passes ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int expect_encode_char_utf8(const char* const file, const int line, const uint32_t code, const char* const expected)
{
    char actual[5] = { 0 };
    encode_char_utf8(actual, code);

    char label[1 << 5] = { 0 };
    (void)snprintf(label, sizeof(label), "U+%x", code);

    const bool passes = str_equals(actual, expected);

    report_status_str(file, line, passes, label, actual, expected);

    return passes ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int expect_decode_char_utf8(const char* const file, const int line, const char* const src, const struct char_descriptor expected)
{
    const struct char_descriptor actual = decode_char_utf8(src);

    char label[1 << 5] = { 0 };
    (void)snprintf(label, sizeof(label), "%.*s", expected.len, src);

    char actual_label[1 << 5] = { 0 };
    (void)snprintf(actual_label, sizeof(actual_label), "code: U+%x, len: %d", actual.code, actual.len);

    char expected_label[1 << 5] = { 0 };
    (void)snprintf(expected_label, sizeof(expected_label), "code: U+%x, len: %d", expected.code, expected.len);

    const bool passes = actual.code == expected.code && actual.len == expected.len;

    report_status(file, line, passes, label, actual_label, expected_label);

    return passes ? EXIT_SUCCESS : EXIT_FAILURE;
}
