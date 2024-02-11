#include "mode.h"
#include "platform.h"
#include "string.h"
#include "time.h"
#include <signal.h>
#include <stdlib.h>

struct config {
    struct {
        enum mode_type type;
        struct mode* value;
    } mode;

    bool help;
    bool version;
    bool license;
};

static const char* configure(struct config* config, unsigned int argc, const char* const* argv);
static void run(enum mode_type type, struct mode* mode);

static int help(void);
static int version(void);
static int lisence(void);

int main(const int argc, const char* const* const argv)
{
    struct mode mode = {
        .ornamental = true,
        .debug = false,
    };

    struct config config = {
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

    run(config.mode.type, &mode);

    deinit_mode(&mode);

    return EXIT_SUCCESS;
}

static const char* read_arg(const char* const* argv, unsigned int* i);
static const char* config_err_no_value_specified(const char* name);

static const char* configure(struct config* const config, const unsigned int argc, const char* const* const argv)
{
    for (unsigned int i = 1; i < argc; i++) {
        const char* const arg = argv[i];

        if (str_equals(arg, "--wabi")) {
            config->mode.type = mode_wabi;
            continue;
        }

        if (str_equals(arg, "--sabi")) {
            const char* const raw = read_arg(argv, &i);
            if (raw == NULL) {
                return config_err_no_value_specified("sabi");
            }
            if (strlen(raw) != 5 /* HH:mm */) {
                return format_str("timer: duration format must be HH:mm");
            }

            struct moment moment = { 0 };
            // NOLINTNEXTLINE(cert-err34-c)
            (void)sscanf(raw, "%2d:%2d", &moment.hours, &moment.mins);

            const struct duration min_duration = duration_from_moment(
                (struct moment) {
                    .mins = 1,
                }
            );
            const struct duration duration = duration_from_moment(moment);
            if (duration.msecs < min_duration.msecs) {
                return format_str("timer: duration must be >= 00:01");
            }

            config->mode.type = mode_sabi;
            config->mode.value->timer.duration = duration;

            continue;
        }

        if (str_equals(arg, "--satori")) {
            config->mode.value->ornamental = false;
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

        return format_str("unknown argument: %s", arg);
    }

    return NULL;
}

static void run(enum mode_type type, struct mode* const mode)
{
    struct sig_handler sig_handler = { 0 };
    watch_sigs(&sig_handler, (unsigned int[]) { SIGINT, SIGTERM }, 2);

    struct mode_ctx ctx = {
        .sig_handler = &sig_handler,
    };

    switch (type) {
    case mode_wabi:
        run_mode_wabi(&ctx, mode);
        break;
    case mode_sabi:
        run_mode_sabi(&ctx, mode);
        break;
    }
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

    print_arg_help("--help", (const char*[]) { "Print help.", NULL });
    print_arg_help("--version", (const char*[]) { "Print version.", NULL });
    print_arg_help("--license", (const char*[]) { "Print license.", NULL });

    return EXIT_SUCCESS;
}

static int version(void)
{
    printf("v0.1.0\n");
    return EXIT_SUCCESS;
}

static int lisence(void)
{
    printf("- MIT License (https://github.com/tomocy/ccodoc/blob/main/LICENSE)\n");
    printf("  things unless otherwise stated\n");
    printf("\n");

    printf("- CC0 1.0 (https://creativecommons.org/publicdomain/zero/1.0/deed.en)\n");
    printf("  - tsutsu_drip sound\n");
    printf("\n");

    printf("- ＮＨＫクリエイティブ･ライブラリー（https://www.nhk.or.jp/archives/creative/rule.html）by NHK\n");
    printf("  - tsutsu_bump sound\n");
    printf("  - uguisu_call sound\n");
    printf("\n");

    return EXIT_SUCCESS;
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

static void print_arg_help(const char* const arg, const char* const* const descs)
{
    printf("%s\n", arg);

    for (const char* const* desc = descs; *desc; desc++) {
        printf("  %s\n", *desc);
    }
    printf("\n");
}
