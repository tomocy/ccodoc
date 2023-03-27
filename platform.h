#pragma once

#define PLATFORM_UNKNOWN 0
#define PLATFORM_LINUX 1
#define PLATFORM_MACOS 2

extern void run_cmd(const char* path, const char* const* args);
