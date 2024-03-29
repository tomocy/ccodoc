#pragma clang diagnostic ignored "-Wgnu-binary-literal"

#include "string.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool str_equals(const char* const str, const char* const other)
{
    return strcmp(str, other) == 0;
}

bool str_equals_n(const char* const str, const char* const other, const unsigned int n)
{
    return strncmp(str, other, n) == 0;
}

bool str_starts_with(const char* const str, const char* const prefix)
{
    return str_equals_n(str, prefix, strlen(prefix));
}

char* copy_str(const char* const str)
{
    const size_t len = strlen(str);
    char* const dst = calloc(len + 1, sizeof(char));
    strncpy(dst, str, len);
    return dst;
}

char* format_str(const char* const format, ...)
{
    va_list args = { 0 };
    va_start(args, format);

    char* data = NULL;
    size_t len = 0;
    FILE* stream = open_memstream(&data, &len);

    (void)vfprintf(stream, format, args);

    (void)fclose(stream);

    va_end(args);

    return data;
}

struct char_descriptor encode_char_utf8(char* const dst, const uint32_t code)
{
    struct char_descriptor desc = { .code = code };

    // 0xxxxxxx
    if (code <= 0x7f) {
        dst[0] = (char)(0b01111111 & code);

        desc.len = 1;

        return desc;
    }

    // 110xxxxx 10xxxxxx
    if (code <= 0x7ff) {
        dst[0] = (char)(0b11000000 | (0b00011111 & (code >> 6)));
        dst[1] = (char)(0b10000000 | (0b00111111 & code));

        desc.len = 2;

        return desc;
    }

    // 1110xxxx 10xxxxxx 10xxxxxx
    if (code <= 0x7fff) {
        dst[0] = (char)(0b11100000 | (0b00001111 & (code >> 12)));
        dst[1] = (char)(0b10000000 | (0b00111111 & (code >> 6)));
        dst[2] = (char)(0b10000000 | (0b00111111 & code));

        desc.len = 3;

        return desc;
    }

    // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    dst[0] = (char)(0b11110000 | (0b00000111 & (code >> 18)));
    dst[1] = (char)(0b10000000 | (0b00111111 & (code >> 12)));
    dst[2] = (char)(0b10000000 | (0b00111111 & (code >> 6)));
    dst[3] = (char)(0b10000000 | (0b00111111 & code));

    desc.len = 4;

    return desc;
}

struct char_descriptor decode_char_utf8(const char* const src)
{
    struct char_descriptor desc = { 0 };

    const unsigned char* const c = (unsigned char*)src;

    if (*c <= 0x7f) {
        desc.code = 0b01111111 & *c;
        desc.len = 1;
        return desc;
    }

    if (*c >= 0b11110000) {
        desc.len = 4;
        desc.code = 0b00000111 & *c;
    } else if (*c >= 0b11100000) {
        desc.len = 3;
        desc.code = 0b00001111 & *c;
    } else if (*c >= 0b11000000) {
        desc.len = 2;
        desc.code = 0b00011111 & *c;
    }

    for (int i = 1; i < desc.len; i++) {
        desc.code = (desc.code << 6) | (0b00111111 & c[i]);
    }

    return desc;
}

void decode_str_utf8(struct char_descriptor* const descs, const char* const src)
{
    int i = 0;
    const char* c = src;

    while (*c) {
        const struct char_descriptor desc = decode_char_utf8(c);
        descs[i] = desc;

        i++;
        c += desc.len;
    }
}
