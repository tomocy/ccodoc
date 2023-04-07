#define _POSIX_C_SOURCE 200809L

#include <stdio.h>

static const char* read_file(char** data, size_t* len, const char* file);

#define EMBED_ASSET(dst, file, s)                                         \
    {                                                                     \
        size_t n = fprintf((dst), s);                                     \
        if (n < 0) {                                                      \
            (void)fprintf(stderr, "failed to embed asset: %s\n", (file)); \
            return 1;                                                     \
        }                                                                 \
    }

#define EMBEDF_ASSET(dst, file, format, ...)                              \
    {                                                                     \
        size_t n = fprintf((dst), (format), __VA_ARGS__);                 \
        if (n < 0) {                                                      \
            (void)fprintf(stderr, "failed to embed asset: %s\n", (file)); \
            return 1;                                                     \
        }                                                                 \
    }

int main(void)
{
    FILE* const dst = fopen("./assets/sounds/sounds.h", "w");
    if (dst == NULL) {
        (void)fprintf(stderr, "failed to open destination file\n");
        return 1;
    }

    {
        const char data[] = "#pragma once\n";
        const size_t len = sizeof(data) - 1;
        const size_t n = fwrite(data, sizeof(char), len, dst);
        if (n != len) {
            (void)fprintf(stderr, "failed to embed assets\n");
            return 1;
        }
    }

    static const struct asset {
        const char* name;
        const char* file;
        const char* license;
    } assets[] = {
        (struct asset) { .name = "sound_tsutsu_drip", "./assets/sounds/tsutsu_drip.mp3", .license = "CC0 1.0" },
        (struct asset) { .name = "sound_tsutsu_bump", "./assets/sounds/tsutsu_bump.mp3", .license = "ＮＨＫクリエイティブ･ライブラリー" },
        (struct asset) { .name = "sound_uguisu_call", "./assets/sounds/uguisu_call.mp3", .license = "ＮＨＫクリエイティブ･ライブラリー" },
    };
    static const size_t assets_len = sizeof(assets) / sizeof(struct asset);

    for (size_t i = 0; i < assets_len; i++) {
        const struct asset asset = assets[i];

        char* data = { 0 };
        size_t len = 0;
        {
            const char* const err = read_file(&data, &len, asset.file);
            if (err != NULL) {
                (void)fprintf(stderr, "failed to read asset: %s: %s\n", asset.file, err);
                return 1;
            }
        }

        EMBED_ASSET(dst, asset.file, "\n");

        EMBEDF_ASSET(dst, asset.file, "// license: %s\n", asset.license);

        EMBEDF_ASSET(dst, asset.file, "const unsigned char %s[] = {", asset.name);

        for (size_t i = 0; i < len; i++) {
            const char* format = i != 0 ? ", 0x%02x" : "0x%02x";
            const unsigned char c = data[i];
            EMBEDF_ASSET(dst, asset.file, format, c);
        }

        EMBED_ASSET(dst, asset.file, "};\n");
    }

    if (fclose(dst) != 0) {
        (void)fprintf(stderr, "failed to close destination file\n");
        return 1;
    }

    return 0;
}

static const char* read_file(char** const data, size_t* const len, const char* const file)
{
    FILE* const src = fopen(file, "r");
    if (src == NULL) {
        return "failed to open file";
    }

    FILE* const stream = open_memstream(data, len);
    if (stream == NULL) {
        return "failed to open buffer";
    }

    while (1) {
        char buf[1 << 12] = { 0 };
        const size_t n_read = fread(buf, sizeof(char), sizeof(buf), src);
        if (n_read == 0) {
            break;
        }

        const size_t n_written = fwrite(buf, sizeof(char), n_read, stream);
        if (n_written != n_read) {
            return "failed to copy to buffer";
        }
    }

    if (fclose(stream) != 0) {
        return "failed to close buffer";
    }
    if (fclose(src) != 0) {
        return "failed to close file";
    }

    return NULL;
}
