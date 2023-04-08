#pragma once

#include <stdbool.h>
#include <stddef.h>

#define PLATFORM_UNKNOWN 0
#define PLATFORM_LINUX 1
#define PLATFORM_MACOS 2

typedef struct {
    int pipe[2];

    struct {
        unsigned int* values;
        size_t len;
    } sigs;
} sig_handler_t;

extern const char* get_user_home_dir(void);
extern const char* get_user_cache_dir(void);
extern const char* get_dir(const char* path);
extern const char* make_dir(const char* name);

extern const char* join_paths(const char* const* paths);

extern bool has_file(const char* path);

extern void run_cmd(const char* path, const char* const* args);

extern const char* watch_sigs(sig_handler_t* handler, unsigned int* sigs, size_t len);
extern const char* catch_sig(const sig_handler_t* handler, unsigned int* sig, bool* caught);
