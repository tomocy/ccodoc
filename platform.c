#include "platform.h"

#include "string.h"
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

const char* get_user_home_dir(void)
{
#if PLATFORM == PLATFORM_LINUX || PLATFORM == PLATFORM_MACOS
    return getenv("HOME");
#else
    return NULL;
#endif
}

const char* get_user_cache_dir(void)
{
#if PLATFORM == PLATFORM_LINUX
    const char* const dir = getenv("XDG_CACHE_HOME");
    if (dir != NULL && !str_equals(dir, "")) {
        return (char*)dir;
    }

    return join_paths((const char*[]) { get_user_home_dir(), ".cache", NULL });
#elif PLATFORM == PLATFORM_MACOS
    return join_paths((const char*[]) { get_user_home_dir(), "Library/Caches", NULL });
#else
    return NULL;
#endif
}

const char* get_dir(const char* path)
{
    char* path2 = copy_str(path);
    const char* dir = dirname(path2);
    if ((void*)dir == (void*)path2) {
        return path2;
    }

    // Some platform such as MacOS (https://opensource.apple.com/source/Libc/Libc-391/gen/FreeBSD/dirname.c)
    // returns the directory name using the same address instead of the given one.
    // This causes the next dirname call to overwrite the previous result, which is error-prune.
    // To avoid this, copy the result to the different address and return it.

    const char* dir2 = copy_str(dir);
    free((void*)path2);

    return dir2;
}

// NOLINTNEXTLINE(misc-no-recursion)
const char* make_dir(const char* name)
{
    if (has_file(name)) {
        return NULL;
    }

    const char* dir = get_dir(name);
    if (!str_equals(name, dir)) {
        const char* err = make_dir(dir);
        if (err != NULL) {
            free((void*)dir);
            return err;
        }
    }

    int status = mkdir(name, 0755);
    if (status != 0) {
        return format_str("%s", name);
    }

    free((void*)dir);

    return NULL;
}

const char* join_paths(const char* const* const paths)
{
    char* x = { 0 };
    size_t len = 0;

    {
        FILE* const buf = open_memstream(&x, &len);

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
