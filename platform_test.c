#include "platform.h"

#include "string.h"
#include "test.h"

static int expect_join_paths(const char* file, int line, const char* const* paths, const char* expected);
#define EXPECT_JOIN_PATHS(paths, expected) EXPECT_PASS(expect_join_paths(__FILE__, __LINE__, paths, expected))

int test_platform(void)
{
    {
        printf("- join_paths\n");

        EXPECT_JOIN_PATHS(((const char*[]) { NULL }), "");
        EXPECT_JOIN_PATHS(((const char*[]) { "", NULL }), "");
        EXPECT_JOIN_PATHS(((const char*[]) { "", "", "", "", NULL }), "");
        EXPECT_JOIN_PATHS(((const char*[]) { "/", NULL }), "/");
        EXPECT_JOIN_PATHS(((const char*[]) { "a", "b", NULL }), "a/b");
        EXPECT_JOIN_PATHS(((const char*[]) { "a/", "/b", NULL }), "a/b");
        EXPECT_JOIN_PATHS(((const char*[]) { "/a//", "//b/", "/c/", NULL }), "/a/b/c");
    }

    return EXIT_SUCCESS;
}

static int expect_join_paths(const char* const file, const int line, const char* const* const paths, const char* const expected)
{
    const char* const actual = join_paths(paths);

    char label[1 << 5] = { 0 };
    {
        int i = 0;
        const char* const* path = paths;
        int n = 0;
        while (*path) {
            if (i != 0) {
                n += snprintf(label + n, sizeof(label) - n, ",");
            }
            n += snprintf(label + n, sizeof(label) - n, "\"%s\"", *path);

            i++;
            path++;
        }
    }

    const bool passes = str_equals(actual, expected);

    report_status_str(file, line, passes, label, actual, expected);

    free((void*)actual);

    return passes ? EXIT_SUCCESS : EXIT_FAILURE;
}
