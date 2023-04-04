#include "canvas.h"
#include "ccodoc.h"
#include "mode.h"
#include "renderer.h"
#include "string.h"
#include "time.h"
#include <stdlib.h>
#include <sys/stat.h>

typedef struct {
    struct {
        ccodoc_mode_type_t type;
        ccodoc_mode_t* value;
    } mode;

    bool help;
    bool version;
    bool license;
} config_t;

static const char* configure(config_t* config, unsigned int argc, const char* const* argv);

static int help(void);
static int version(void);
static int lisence(void);

int main(const int argc, const char* const* const argv)
{
    ccodoc_mode_t mode = {
        .ornamental = true,
        .debug = false,
    };

    config_t config = {
        .mode = {
            .type = mode_wabi,
            .value = &mode,
        },
    };

    {
        const char* const err = configure(&config, argc, argv);
        if (err != NULL) {
            help();
            printf("\n");
            (void)fprintf(stderr, "invalid options: %s\n", err);
            free((void*)err);
            return EXIT_FAILURE;
        }
    }

    if (config.help) {
        return help();
    }
    if (config.version) {
        return version();
    }
    if (config.license) {
        return lisence();
    }

    init_mode(&mode);

    switch (config.mode.type) {
    case mode_wabi:
        run_mode_wabi(&mode);
        break;
    case mode_sabi:
        run_mode_sabi(&mode);
        break;
    }

    deinit_mode(&mode);

    return EXIT_SUCCESS;
}

static const char* read_arg(const char* const* argv, unsigned int* i);
static const char* config_err_no_value_specified(const char* name);
static const char* config_err_no_file_found(const char* name);

#define READ_ARG_FILE(dst, argv, i, name)               \
    {                                                   \
        const char** dst_ = (dst);                      \
        const char* const* const argv_ = (argv);        \
        unsigned int* i_ = (i);                         \
                                                        \
        const char* const file = read_arg(argv_, i_);   \
        if (file == NULL) {                             \
            return config_err_no_value_specified(name); \
        }                                               \
        if (!has_file(file)) {                          \
            return config_err_no_file_found(name);      \
        }                                               \
                                                        \
        *dst_ = file;                                   \
    }

static const char* configure(config_t* const config, const unsigned int argc, const char* const* const argv)
{
    for (unsigned int i = 1; i < argc; i++) {
        const char* const arg = argv[i];

        if (str_equals(arg, "--sabi")) {
            const char* const raw = read_arg(argv, &i);
            if (raw == NULL) {
                return config_err_no_value_specified("sabi");
            }

            moment_t m = { 0 };
            // NOLINTNEXTLINE(cert-err34-c)
            (void)sscanf(raw, "%d:%d", &m.hours, &m.mins);

            const duration_t d = duration_from_moment(m);
            if (d.msecs == 0) {
                return format_str("timer: duration format must be HH:mm");
            }

            config->mode.type = mode_sabi;
            config->mode.value->timer.duration = d;

            continue;
        }

        if (str_equals(arg, "--satori")) {
            config->mode.value->ornamental = false;
            continue;
        }

        if (str_equals(arg, "--sound-tsutsu-drip")) {
            READ_ARG_FILE(&config->mode.value->sound.tsutsu_drip, argv, &i, "sound-tsutsu-drip");
            continue;
        }

        if (str_equals(arg, "--sound-tsutsu-bump")) {
            READ_ARG_FILE(&config->mode.value->sound.tsutsu_bump, argv, &i, "sound-tsutsu-bump");
            continue;
        }

        if (str_equals(arg, "--sound-uguisu-call")) {
            READ_ARG_FILE(&config->mode.value->sound.uguisu_call, argv, &i, "sound-uguisu-call");
            continue;
        }

        if (str_equals(arg, "--help")) {
            config->help = true;
            continue;
        }

        if (str_equals(arg, "--version")) {
            config->version = true;
            continue;
        }

        if (str_equals(arg, "--license")) {
            config->license = true;
            continue;
        }

        if (str_equals(arg, "--debug")) {
            config->mode.value->debug = true;
            continue;
        }
    }

    return NULL;
}

static const char* read_arg(const char* const* const argv, unsigned int* const i)
{
    const unsigned int next_i = *i + 1;
    const char* next_arg = argv[next_i];
    if (!next_arg) {
        return NULL;
    }

    *i = next_i;
    return next_arg;
}

static const char* config_err_no_value_specified(const char* const name)
{
    return format_str("%s: value must be specified", name);
}

static const char* config_err_no_file_found(const char* const name)
{
    return format_str("%s: file not found", name);
}

static void print_arg_help(const char* arg, const char* const* descs);

static int help(void)
{
    printf("# ccodoc\n");

    printf("\nccodoc（鹿威し）simulated（造）in your terminal（庭）\n");

    printf("\n\n## options\n\n");

    print_arg_help(
        "--wabi",
        (const char*[]) {
            "Render ccodoc alone. (default)",
            "Unsatisfied does not always mean not-satisfied. Absent does not always mean not-present.",
            "It is just what it is.（侘び）",
            NULL,
        }
    );

    print_arg_help(
        "--sabi HH:mm",
        (const char*[]) {
            "Render ccodoc with the timer set for this duration.",
            "Impermanent does not always mean not-permanent. Behind does not always mean not-ahead.",
            "It will be just what it will be.（寂び）",
            NULL,
        }
    );

    print_arg_help(
        "--satori",
        (const char*[]) {
            "Remove all ornaments.",
            "Neither color nor sound is needed as they already lie within you.（悟り）",
            NULL,
        }
    );

    print_arg_help(
        "--sound-tsutsu-drip file",
        (const char*[]) { "Play this sound when water drips from kakehi（筧）into tsutsu（筒）.", NULL }
    );
    print_arg_help(
        "--sound-tsutsu-bump file",
        (const char*[]) { "Play this sound when tsutsu（筒）bumps.", NULL }
    );
    print_arg_help(
        "--sound-uguisu-call file",
        (const char*[]) {
            "Play this sound when uguisu（鶯）calls.",
            "Uguisu calls when the timer expires in sabi mode, etc.",
            NULL,
        }
    );

    print_arg_help("--help", (const char*[]) { "Print help.", NULL });
    print_arg_help("--version", (const char*[]) { "Print version.", NULL });
    print_arg_help("--license", (const char*[]) { "Print license.", NULL });

    return EXIT_SUCCESS;
}

static void print_arg_help(const char* const arg, const char* const* const descs)
{
    printf("%s\n", arg);

    for (const char* const* desc = descs; *desc; desc++) {
        printf("  %s\n", *desc);
    }
    printf("\n");
}

static int version(void)
{
    printf("v0.0.0\n");
    return EXIT_SUCCESS;
}

static int lisence(void)
{
    printf("- MIT License (https://github.com/tomocy/ccodoc/blob/main/LICENSE)\n");
    printf("  things unless otherwise stated\n");
    printf("\n");

    printf("- CC0 1.0 (https://creativecommons.org/publicdomain/zero/1.0/deed.en)\n");
    printf("  - tsutsu_drip sound\n");
    printf("  - tsutsu_bump sound\n");
    printf("\n");

    printf("- ＮＨＫクリエイティブ･ライブラリー（https://www.nhk.or.jp/archives/creative/rule.html）by NHK\n");
    printf("  - uguisu_call sound\n");
    printf("\n");

    return EXIT_SUCCESS;
}
