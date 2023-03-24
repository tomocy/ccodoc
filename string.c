#include "string.h"

#include <string.h>

bool str_equals(const char* str, const char* other)
{
    return strcmp(str, other) == 0;
}

bool str_equals_n(const char* str, const char* other, unsigned int n)
{
    return strncmp(str, other, n) == 0;
}

size_t decode_char_utf8(const char* src)
{
    unsigned char c = *src;

    // ASCII
    if (c <= 0x7f) {
        return 1;
    }

    // The first byte in UTF-8 indicates how long the encoded bytes are.

    if (c >= /* 0b11110000 */ 240) {
        return 4;
    }
    if (c >= /* 0b11100000 */ 224) {
        return 3;
    }
    if (c >= /* 0b11000000 */ 192) {
        return 2;
    }

    return 0;
}
