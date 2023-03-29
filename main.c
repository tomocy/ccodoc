#include "canvas.h"
#include "ccodoc.h"
#include "mode.h"
#include "renderer.h"
#include "string.h"
#include "time.h"
#include <stdlib.h>

typedef struct {
    mode_type_t mode;

    struct {
        duration_t duration;
    } sabi;

    bool satori;

    struct {
        const char* tsutsu_poured;
        const char* tsutsu_bumped;
    } sound;

    bool help;
    bool debug;
} config_t;

static const char* configure_with_args(config_t* config, int argc, const char* const* argv);
static int help(void);

int main(const int argc, const char* const* const argv)
{
    config_t config = {
        .mode = mode_wabi,
        .satori = false,
        .help = false,
        .debug = false,
    };

    {
        const char* const err = configure_with_args(&config, argc, argv);
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

    mode_t mode = { .type = config.mode };

    {
        mode_opt_general_t general_opt = {
            .ornamental = !config.satori,
            .sound = {
                .tsutsu_poured = config.sound.tsutsu_poured,
                .tsutsu_bumped = config.sound.tsutsu_bumped,
            },
            .debug = config.debug,
        };

        mode_opt_t opt = { .type = mode.type };
        switch (mode.type) {
        case mode_wabi:
            opt.delegate.wabi = (mode_opt_wabi_t) { .general = general_opt };
            break;
        case mode_sabi:
            opt.delegate.sabi = (mode_opt_sabi_t) {
                .general = general_opt,
                .duration = config.sabi.duration,
            };
            break;
        }

        init_mode(&mode, opt);
    }

    run_mode(&mode);

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

static const char* configure_with_args(config_t* const config, const int argc, const char* const* const argv)
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

            config->mode = mode_sabi;
            config->sabi.duration = d;

            continue;
        }

        if (str_equals(arg, "--satori")) {
            config->satori = true;
            continue;
        }

        if (str_equals(arg, "--sound-tsutsu-poured")) {
            const char* const name = read_arg(&i, argv);
            if (name == NULL) {
                return CONFIG_ERR_NO_VALUE_SPECIFIED("sound-tsutsu-poured");
            }

            config->sound.tsutsu_poured = name;

            continue;
        }

        if (str_equals(arg, "--sound-tsutsu-bumped")) {
            const char* const name = read_arg(&i, argv);
            if (name == NULL) {
                return CONFIG_ERR_NO_VALUE_SPECIFIED("sound-tsutsu-bumped");
            }

            config->sound.tsutsu_bumped = name;

            continue;
        }

        if (str_equals(arg, "--help")) {
            config->help = true;
            continue;
        }

        if (str_equals(arg, "--debug")) {
            config->debug = true;
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
            "Neither color nor sound is needed as they already lies within you.（悟り）",
            NULL,
        }
    );

    print_arg_help(
        "--sound-tsutsu-poured file",
        (const char*[]) { "Play this sound on tsutsu（筒）poured.", NULL }
    );
    print_arg_help(
        "--sound-tsutsu-bumped file",
        (const char*[]) { "Play this sound on tsutsu（筒）bumped.", NULL }
    );

    print_arg_help("--help", (const char*[]) { "Print help.", NULL });

    return EXIT_SUCCESS;
}
