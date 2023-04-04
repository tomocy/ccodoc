#include "platform.h"

#include "string.h"
#include "test.h"

int test_platform(void)
{
    {
        printf("- join_paths\n");

        {
            const char* paths = join_paths((const char*[]) { NULL });
            EXPECT_TRUE_X_L("join_paths({NULL}) == \"\"", str_equals(paths, ""));
            free((void*)paths);
        }

        {
            const char* paths = join_paths((const char*[]) { "", NULL });
            EXPECT_TRUE_X_L("join_paths({\"\", NULL}) == \"\"", str_equals(paths, ""));
            free((void*)paths);
        }

        {
            const char* paths = join_paths((const char*[]) { "", "", "", "", NULL });
            EXPECT_TRUE_X_L("join_paths({\"\", \"\", \"\", \"\", NULL}) == \"\"", str_equals(paths, ""));
            free((void*)paths);
        }

        {
            const char* paths = join_paths((const char*[]) { "/", NULL });
            EXPECT_TRUE_X_L("join_paths({\"/\", NULL}) == \"/\"", str_equals(paths, "/"));
            free((void*)paths);
        }

        {
            const char* paths = join_paths((const char*[]) { "a", "b", NULL });
            EXPECT_TRUE_X_L("join_paths({\"a\", \"b\", NULL}) == \"a/b\"", str_equals(paths, "a/b"));
            free((void*)paths);
        }

        {
            const char* paths = join_paths((const char*[]) { "a/", "/b", NULL });
            EXPECT_TRUE_X_L("join_paths({\"a/\", \"/b\", NULL}) == \"a/b\"", str_equals(paths, "a/b"));
            free((void*)paths);
        }

        {
            const char* paths = join_paths((const char*[]) { "/a//", "//b/", "/c/", NULL });
            EXPECT_TRUE_X_L("join_paths({\"/a//\", \"//b/\", \"/c/\", NULL}) == \"a/b\"", str_equals(paths, "/a/b/c"));
            free((void*)paths);
        }
    }

    return EXIT_SUCCESS;
}
