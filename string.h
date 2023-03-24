#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint32_t code;
    int len;
} char_descriptor;

extern bool str_equals(const char* str, const char* other);
extern bool str_equals_n(const char* str, const char* other, unsigned int n);

extern char_descriptor encode_char_utf8(char* dst, uint32_t code);
extern char_descriptor decode_char_utf8(const char* src);
extern void decode_str_utf8(char_descriptor* codes, const char* src);
