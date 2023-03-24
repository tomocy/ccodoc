#pragma once

#include <stdbool.h>
#include <stddef.h>

extern bool str_equals(const char* str, const char* other);
extern bool str_equals_n(const char* str, const char* other, unsigned int n);
extern size_t decode_char_utf8(const char* src);
