#define _GNU_SOURCE

#include "platform.h"

#include "string.h"
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
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
    char* const path2 = copy_str(path);
    const char* const dir = dirname(path2);
    if ((void*)dir == (void*)path2) {
        return path2;
    }

    // Some platform such as MacOS (https://opensource.apple.com/source/Libc/Libc-391/gen/FreeBSD/dirname.c)
    // returns the directory name using the same address instead of the given one.
    // This causes the next dirname call to overwrite the previous result, which is error-prune.
    // To avoid this, copy the result to the different address and return it.

    const char* const dir2 = copy_str(dir);
    free((void*)path2);

    return dir2;
}

// NOLINTNEXTLINE(misc-no-recursion)
const char* make_dir(const char* name)
{
    if (has_file(name)) {
        return NULL;
    }

    const char* const dir = get_dir(name);
    if (!str_equals(name, dir)) {
        const char* const err = make_dir(dir);
        if (err != NULL) {
            free((void*)dir);
            return err;
        }
    }

    const int status = mkdir(name, 0755);
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
            const size_t n = fprintf(buf, i != 0 ? "/%s" : "%s", *path);
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

static const char* prepare_sig_set(sigset_t* sig_set, unsigned int* sigs, size_t len);
static bool watches_sig(const struct sig_handler* const handler, unsigned int sig);
static void* wait_sigs(const struct sig_handler* const handler);

static const char* read_sig(const struct sig_handler* const handler, unsigned int* const sig);
static const char* write_sig(const struct sig_handler* const handler, const unsigned int* const sig);
static int sig_pipe_read(const struct sig_handler* handler);
static int sig_pipe_write(const struct sig_handler* handler);

const char* watch_sigs(struct sig_handler* const handler, unsigned int* const sigs, const size_t len)
{
    handler->sigs.values = sigs;
    handler->sigs.len = len;

    {
        errno = 0;
#if PLATFORM != PLATFORM_MACOS
        const int status = pipe2(handler->pipe, O_CLOEXEC);
        if (status != 0) {
            return format_str("failed to init pipe: %d", errno);
        }
#else
        {
            const int status = pipe(handler->pipe);
            if (status != 0) {
                return format_str("failed to init pipe: %d", errno);
            }
        }
        {
            const int status = fcntl(handler->pipe[0], F_SETFD, FD_CLOEXEC);
            if (status != 0) {
                return format_str("failed to init pipe: %d", errno);
            }
        }
        {
            const int status = fcntl(handler->pipe[1], F_SETFD, FD_CLOEXEC);
            if (status != 0) {
                return format_str("failed to init pipe: %d", errno);
            }
        }
#endif
    }

    sigset_t sig_set = { 0 };
    {
        const char* const err = prepare_sig_set(&sig_set, sigs, len);
        if (err != NULL) {
            const char* const err2 = format_str("failed to prepare signal set: %s", err);
            free((void*)err);
            return err2;
        }
    }

    pthread_t thread = { 0 };
    {
        errno = 0;
        const int status = pthread_create(&thread, NULL, (void* (*)(void*))wait_sigs, handler);
        if (status != 0) {
            return format_str("failed to create signal thread: %d", errno);
        }
    }
    {
        errno = 0;
        const int status = pthread_detach(thread);
        if (status != 0) {
            return format_str("failed to detach signal thread: %d", errno);
        }
    }

    return NULL;
}

const char* catch_sig(const struct sig_handler* const handler, unsigned int* const sig, bool* const caught)
{
    fd_set fds = { 0 };
    FD_ZERO(&fds);

    const int pipe = sig_pipe_read(handler);
    FD_SET(pipe, &fds);

    while (true) {
        {
            fd_set fds_read = fds;

            errno = 0;
            const struct timespec timeout = { 0 };
            const int n = pselect(pipe + 1, &fds_read, NULL, NULL, &timeout, NULL);
            if (n < 0) {
                if (errno == EINTR || errno == EAGAIN) {
                    continue;
                }

                return format_str("failed to select signal pipe: %d", errno);
            }

            if (n == 0 || !FD_ISSET(pipe, &fds_read)) {
                *caught = false;
                return NULL;
            }
        }

        unsigned int sig2 = 0;
        {
            const char* const err = read_sig(handler, &sig2);
            if (err != NULL) {
                const char* const err2 = format_str("failed to read signal: %s", err);
                free((void*)err);
                return err2;
            }
        }

        *sig = sig2;
        *caught = true;
        break;
    }

    return NULL;
}

static const char* prepare_sig_set(sigset_t* const sig_set, unsigned int* const sigs, const size_t len)
{
    {
        errno = 0;
        const int status = sigemptyset(sig_set);
        if (status != 0) {
            return format_str("failed to prepare signal set: %d", errno);
        }
    }
    for (size_t i = 0; i < len; i++) {
        const unsigned int sig = sigs[i];

        errno = 0;
        const int status = sigaddset(sig_set, (int)sig);
        if (status != 0) {
            return format_str("failed to set target signal: %d: %d", sig, errno);
        }
    }
    {
        errno = 0;
        const int status = sigprocmask(SIG_BLOCK, sig_set, NULL);
        if (status != 0) {
            return format_str("failed to mask signal set: %d", errno);
        }
    }

    return NULL;
}

static bool watches_sig(const struct sig_handler* const handler, const unsigned int sig)
{
    for (size_t i = 0; i < handler->sigs.len; i++) {
        if (sig == handler->sigs.values[i]) {
            return true;
        }
    }

    return false;
}

static void* wait_sigs(const struct sig_handler* const handler)
{
    sigset_t sig_set = { 0 };
    prepare_sig_set(&sig_set, handler->sigs.values, handler->sigs.len);

    while (true) {
        unsigned int sig = 0;

        errno = 0;
        const int status = sigwait(&sig_set, (int*)&sig);
        if (status < 0) {
            continue;
        }

        if (!watches_sig(handler, sig)) {
            continue;
        }

        write_sig(handler, &sig);
    }

    return NULL;
}

static const char* read_sig(const struct sig_handler* const handler, unsigned int* const sig)
{
    static const size_t len = sizeof(unsigned int);
    unsigned int n_read = 0;

    const int pipe = sig_pipe_read(handler);

    while (n_read < len) {
        errno = 0;
        const size_t n = read(pipe, sig + n_read, len - n_read);
        if (n < 0) {
            if (errno == EINTR || errno == EAGAIN) {
                continue;
            }

            return format_str("%d", errno);
        }
        if (n == 0) {
            break;
        }

        n_read += n;
    }

    return NULL;
}

static const char* write_sig(const struct sig_handler* const handler, const unsigned int* const sig)
{
    static const size_t len = sizeof(unsigned int);
    unsigned int n_written = 0;

    const int pipe = sig_pipe_write(handler);

    while (n_written < len) {
        errno = 0;
        const size_t n = write(pipe, sig + n_written, len - n_written);
        if (n < 0) {
            if (errno == EINTR || errno == EAGAIN) {
                continue;
            }

            return format_str("%d", errno);
        }
        if (n == 0) {
            break;
        }

        n_written += n;
    }

    return NULL;
}

static int sig_pipe_read(const struct sig_handler* handler)
{
    return handler->pipe[0];
}

static int sig_pipe_write(const struct sig_handler* handler)
{
    return handler->pipe[1];
}
