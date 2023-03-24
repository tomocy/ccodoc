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

char_descriptor encode_char_utf8(char* dst, uint32_t code)
{
    char_descriptor desc = { .code = code };

    // 0xxxxxxx
    if (code <= 0x7f) {
        dst[0] = /* 0b01111111 */ (char)(127 & code);

        desc.len = 1;

        return desc;
    }

    // 110xxxxx 10xxxxxx
    if (code <= 0x7ff) {
        dst[0] = (char)(/* 0b11000000 */ 192 | (/* 0b00011111 */ 31 & (code >> 6)));
        dst[1] = (char)(/* 0b10000000 */ 128 | (/* 0b00111111 */ 63 & code));

        desc.len = 2;

        return desc;
    }

    // 1110xxxx 10xxxxxx 10xxxxxx
    if (code <= 0x7fff) {
        dst[0] = (char)(/* 0b11100000 */ 224 | (/* 0b00001111 */ 15 & (code >> 12)));
        dst[1] = (char)(/* 0b10000000 */ 128 | (/* 0b00111111 */ 63 & (code >> 6)));
        dst[2] = (char)(/* 0b10000000 */ 128 | (/* 0b00111111 */ 63 & code));

        desc.len = 3;

        return desc;
    }

    // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    dst[0] = (char)(/* 0b11110000 */ 240 | (/* 0b00000111 */ 7 & (code >> 18)));
    dst[1] = (char)(/* 0b10000000 */ 128 | (/* 0b00111111 */ 63 & (code >> 12)));
    dst[2] = (char)(/* 0b10000000 */ 128 | (/* 0b00111111 */ 63 & (code >> 6)));
    dst[3] = (char)(/* 0b10000000 */ 128 | (/* 0b00111111 */ 63 & code));

    desc.len = 4;

    return desc;
}

char_descriptor decode_char_utf8(const char* src)
{
    char_descriptor desc = { 0 };

    const unsigned char* c = (unsigned char*)src;

    if (*c <= 0x7f) {
        desc.code = /* 0b01111111 */ 127 & *c;
        desc.len = 1;
        return desc;
    }

    if (*c >= /* 0b11110000 */ 240) {
        desc.len = 4;
        desc.code = /* 0b00000111 */ 7 & *c;
    } else if (*c >= /* 0b11100000 */ 224) {
        desc.len = 3;
        desc.code = /* 0b00001111 */ 15 & *c;
    } else if (*c >= /* 0b11000000 */ 192) {
        desc.len = 2;
        desc.code = /* 0b00011111 */ 31 & *c;
    }

    for (int i = 1; i < desc.len; i++) {
        desc.code = (desc.code << 6) | (/* 0b00111111 */ 63 & c[i]);
    }

    return desc;
}

void decode_str_utf8(char_descriptor* descs, const char* src)
{
    int i = 0;
    const char* c = src;

    while (*c) {
        const char_descriptor desc = decode_char_utf8(c);
        descs[i] = desc;

        i++;
        c += desc.len;
    }
}
