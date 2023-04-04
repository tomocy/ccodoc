#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint32_t code;
    int len;
} char_descriptor_t;

extern bool str_equals(const char* str, const char* other);
extern bool str_equals_n(const char* str, const char* other, unsigned int n);
extern bool str_starts_with(const char* str, const char* prefix);

extern char* format_str(const char* format, ...);

extern char_descriptor_t encode_char_utf8(char* dst, uint32_t code);
extern char_descriptor_t decode_char_utf8(const char* src);
extern void decode_str_utf8(char_descriptor_t* codes, const char* src);
