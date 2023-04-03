#define _POSIX_C_SOURCE 200809L

#include "platform.h"

#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

char* user_home_dir(void)
{
#if PLATFORM == PLATFORM_LINUX || PLATFORM_MACOS
    return getenv("HOME");
#endif
}

char* join_paths(const char* const* const paths)
{
    char* x = { 0 };
    size_t len = 0;

    {
        FILE* buf = open_memstream(&x, &len);

        int i = 0;
        const char* const* path = paths;
        while (*path != NULL) {
            size_t n = fprintf(buf, i != 0 ? "/%s" : "%s", *path);
            if (n < 0) {
                return NULL;
            }

            i++;
            path++;
        }

        (void)fclose(buf);
    }

    {
        size_t i = 0;
        size_t j = 0;
        while (x[j]) {
            while (x[j] == '/' && (x[j + 1] && x[j + 1] == '/')) {
                j++;
            }

            x[i] = x[j];
            i++;
            j++;
        }

        if (len > 1 && x[i - 1] == '/') {
            x[i - 1] = 0;
        }

        while (i < len) {
            x[i] = 0;
            i++;
        }
    }

    return x;
}

bool has_file(const char* const path)
{
    struct stat s = { 0 };
    return stat(path, &s) == 0;
}

void run_cmd(const char* const path, const char* const* const args)
{
#if PLATFORM == PLATFORM_LINUX || PLATFORM == PLATFORM_MACOS
    const int child_pid = fork();
    if (child_pid == -1) {
        return;
    }

    if (child_pid == 0) {
        const int gchild_pid = fork();
        if (gchild_pid == -1) {
            exit(1);
            return;
        }

        if (gchild_pid == 0) {
            execv(path, (char* const*)args);

            exit(1);
            return;
        }

        exit(0);
        return;
    }

    int status = 0;
    waitpid(child_pid, &status, 0);
#endif
}
