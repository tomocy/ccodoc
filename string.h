#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

struct char_descriptor {
    uint32_t code;
    int len;
};

extern bool str_equals(const char* str, const char* other);
extern bool str_equals_n(const char* str, const char* other, unsigned int n);
extern bool str_starts_with(const char* str, const char* prefix);
extern char* copy_str(const char* str);

extern char* format_str(const char* format, ...);

extern struct char_descriptor encode_char_utf8(char* dst, uint32_t code);
extern struct char_descriptor decode_char_utf8(const char* src);
extern void decode_str_utf8(struct char_descriptor* codes, const char* src);
