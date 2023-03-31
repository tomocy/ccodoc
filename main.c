#include "canvas.h"
#include "ccodoc.h"
#include "mode.h"
#include "renderer.h"
#include "string.h"
#include "time.h"
#include <stdlib.h>

typedef struct {
    struct {
        ccodoc_mode_type_t type;
        ccodoc_mode_t* value;
    } mode;

    bool help;
} config_t;

static const char* configure(config_t* config, int argc, const char* const* argv);
static int help(void);

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
        .help = false,
    };

    {
        const char* const err = configure(&config, argc, argv);
        if (err != NULL) {
            help();
            printf("\n");
            (void)fprintf(stderr, "invalid options: %s\n", err);
            return EXIT_FAILURE;
        }
    }

    if (config.help) {
        return help();
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

static const char* read_arg(int* const i, const char* const* const argv)
{
    const int next_i = *i + 1;
    const char* next_arg = argv[next_i];
    if (!next_arg) {
        return NULL;
    }

    *i = next_i;
    return next_arg;
}

#define CONFIG_ERR_NO_VALUE_SPECIFIED(label) label ": value must be specified"

static const char* configure(config_t* const config, const int argc, const char* const* const argv)
{
    for (int i = 1; i < argc; i++) {
        const char* const arg = argv[i];

        if (str_equals(arg, "--sabi")) {
            const char* const raw = read_arg(&i, argv);
            if (raw == NULL) {
                return CONFIG_ERR_NO_VALUE_SPECIFIED("sabi");
            }

            moment_t m = { 0 };
            // NOLINTNEXTLINE(cert-err34-c)
            (void)sscanf(raw, "%d:%d", &m.hours, &m.mins);

            const duration_t d = duration_from_moment(m);
            if (d.msecs == 0) {
                return "timer: duration format must be HH:mm";
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
            const char* const file = read_arg(&i, argv);
            if (file == NULL) {
                return CONFIG_ERR_NO_VALUE_SPECIFIED("sound-tsutsu-drip");
            }

            config->mode.value->sound.tsutsu_drip = file;

            continue;
        }

        if (str_equals(arg, "--sound-tsutsu-bump")) {
            const char* const file = read_arg(&i, argv);
            if (file == NULL) {
                return CONFIG_ERR_NO_VALUE_SPECIFIED("sound-tsutsu-bump");
            }

            config->mode.value->sound.tsutsu_bump = file;

            continue;
        }

        if (str_equals(arg, "--sound-uguisu-call")) {
            const char* const file = read_arg(&i, argv);
            if (file == NULL) {
                return CONFIG_ERR_NO_VALUE_SPECIFIED("sound-uguisu-call");
            }

            config->mode.value->sound.uguisu_call = file;

            continue;
        }

        if (str_equals(arg, "--help")) {
            config->help = true;
            continue;
        }

        if (str_equals(arg, "--debug")) {
            config->mode.value->debug = true;
            continue;
        }
    }

    return NULL;
}

static void print_arg_help(const char* const arg, const char** const descs)
{
    printf("%s\n", arg);

    for (const char** desc = descs; *desc; desc++) {
        printf("  %s\n", *desc);
    }
    printf("\n");
}

static int help(void)
{
    printf("# ccodoc\n");

    printf("\nccodoc（鹿威し）simulated（造）in your terminal（庭）\n");

    printf("\n\n## options\n\n");

    print_arg_help(
        "--wabi",
        (const char*[]) {
            "Render ccodoc alone. (default)",
            "Imperfect does not always mean not-perfect. Simple does not always mean not-complicated.",
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
        (const char*[]) { "Play this sound when water drips into tsutsu（筒）.", NULL }
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

    return EXIT_SUCCESS;
}
