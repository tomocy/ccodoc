#include "platform.h"

#include "ccodoc_test.h"
#include "string.h"

int test_platform(void)
{
    {
        printf("- join_paths\n");

        {
            const char* paths = join_paths((const char*[]) { NULL });
            EXPECT_TRUE(str_equals(paths, ""));
            free((void*)paths);
        }

        {
            const char* paths = join_paths((const char*[]) { "", NULL });
            EXPECT_TRUE(str_equals(paths, ""));
            free((void*)paths);
        }

        {
            const char* paths = join_paths((const char*[]) { "", "", "", "", NULL });
            EXPECT_TRUE(str_equals(paths, ""));
            free((void*)paths);
        }

        {
            const char* paths = join_paths((const char*[]) { "/", NULL });
            EXPECT_TRUE(str_equals(paths, "/"));
            free((void*)paths);
        }

        {
            const char* paths = join_paths((const char*[]) { "a", "b", NULL });
            EXPECT_TRUE(str_equals(paths, "a/b"));
            free((void*)paths);
        }

        {
            const char* paths = join_paths((const char*[]) { "a/", "/b", NULL });
            EXPECT_TRUE(str_equals(paths, "a/b"));
            free((void*)paths);
        }

        {
            const char* paths = join_paths((const char*[]) { "/a//", "//b/", "/c/", NULL });
            EXPECT_TRUE(str_equals(paths, "/a/b/c"));
            free((void*)paths);
        }
    }

    return EXIT_SUCCESS;
}
