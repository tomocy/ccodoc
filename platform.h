#pragma once

#include <stdbool.h>

#define PLATFORM_UNKNOWN 0
#define PLATFORM_LINUX 1
#define PLATFORM_MACOS 2

extern char* user_home_dir(void);
extern char* user_cache_dir(void);
extern char* join_paths(const char* const* paths);
extern bool has_file(const char* path);
extern void run_cmd(const char* path, const char* const* args);
