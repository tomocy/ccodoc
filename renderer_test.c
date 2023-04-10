#include "renderer.h"

#include "math.h"
#include "test.h"
#include "time.h"
#include <assert.h>
#include <stdio.h>

static void tick_for(struct duration delta, struct ccodoc* ccodoc, struct timer* timer);

static int expect_canvas(const char* file, int line, const struct canvas_buffer* actual, const char* expected);
#define EXPECT_CANVAS(actual, expected) EXPECT_PASS(expect_canvas(__FILE__, __LINE__, actual, expected))

int test_renderer(void)
{
    {
        printf("## ccodoc\n");

        struct canvas_buffer canvas_buffer = { 0 };
        init_canvas_buffer(&canvas_buffer, (struct vec2d) { .x = 14 + 2, .y = 6 + 2 });

        struct canvas canvas = wrap_canvas_buffer(&canvas_buffer);

        struct renderer renderer = { .canvas = &canvas };

        struct ccodoc ccodoc = {
            .kakehi = {
                .release_water_amount = 2,
                .holding_water = {
                    .duration = { .msecs = 2100 },
                },
                .releasing_water = {
                    .duration = { .msecs = 900 },
                },
            },
            .tsutsu = {
                .water_capacity = 10,
                .releasing_water = {
                    .duration = { .msecs = 1200 },
                },
            },
            .hachi = {
                .releasing_water = {
                    .duration = { .msecs = 1000 },
                },
            },
        };

        static struct test {
            struct duration delta;
            const char* expected;
        } tests[] = {
            (struct test) {
                .delta = { .msecs = 0 },
                .expected = "                "
                            " ━══            "
                            "    ◥◣          "
                            "      ◥◣        "
                            "      ▕ ◥◣      "
                            "      ▕   ◥◣    "
                            " ▭▭▭▭━━━━━━▨▨▨▨ "
                            "                ",
            },
            (struct test) {
                .delta = { .msecs = 350 },
                .expected = "                "
                            " ━══            "
                            "    ◥◣          "
                            "      ◥◣        "
                            "      ▕ ◥◣      "
                            "      ▕   ◥◣    "
                            " ▭▭▭▭━━━━━━▨▨▨▨ "
                            "                ",
            },
            (struct test) {
                .delta = { .msecs = 350 },
                .expected = "                "
                            " ═━═            "
                            "    ◥◣          "
                            "      ◥◣        "
                            "      ▕ ◥◣      "
                            "      ▕   ◥◣    "
                            " ▭▭▭▭━━━━━━▨▨▨▨ "
                            "                ",
            },
            (struct test) {
                .delta = { .msecs = 350 },
                .expected = "                "
                            " ═━═            "
                            "    ◥◣          "
                            "      ◥◣        "
                            "      ▕ ◥◣      "
                            "      ▕   ◥◣    "
                            " ▭▭▭▭━━━━━━▨▨▨▨ "
                            "                ",
            },
            (struct test) {
                .delta = { .msecs = 350 },
                .expected = "                "
                            " ══━            "
                            "    ◥◣          "
                            "      ◥◣        "
                            "      ▕ ◥◣      "
                            "      ▕   ◥◣    "
                            " ▭▭▭▭━━━━━━▨▨▨▨ "
                            "                ",
            },
            (struct test) {
                .delta = { .msecs = 350 },
                .expected = "                "
                            " ══━            "
                            "    ◥◣          "
                            "      ◥◣        "
                            "      ▕ ◥◣      "
                            "      ▕   ◥◣    "
                            " ▭▭▭▭━━━━━━▨▨▨▨ "
                            "                ",
            },
            (struct test) {
                .delta = { .msecs = 350 },
                .expected = "                "
                            " ═══            "
                            "    ◥◣          "
                            "      ◥◣        "
                            "      ▕ ◥◣      "
                            "      ▕   ◥◣    "
                            " ▭▭▭▭━━━━━━▨▨▨▨ "
                            "                ",
            },
            (struct test) {
                .delta = { .msecs = 450 },
                .expected = "                "
                            " ═══            "
                            "    ◥◣          "
                            "      ◥◣        "
                            "      ▕ ◥◣      "
                            "      ▕   ◥◣    "
                            " ▭▭▭▭━━━━━━▨▨▨▨ "
                            "                ",
            },
            (struct test) {
                .delta = { .msecs = 450 },
                .expected = "                "
                            " ━══            "
                            "    ◥◣          "
                            "      ◥◣        "
                            "      ▕ ◥◣      "
                            "      ▕   ◥◣    "
                            " ▭▭▭▭━━━━━━▨▨▨▨ "
                            "                ",
            },
            (struct test) {
                .delta = { .msecs = 700 },
                .expected = "                "
                            " ═━═            "
                            "    ◥◣          "
                            "      ◥◣        "
                            "      ▕ ◥◣      "
                            "      ▕   ◥◣    "
                            " ▭▭▭▭━━━━━━▨▨▨▨ "
                            "                ",
            },
            (struct test) {
                .delta = { .msecs = 700 },
                .expected = "                "
                            " ══━            "
                            "    ◥◣          "
                            "      ◥◣        "
                            "      ▕ ◥◣      "
                            "      ▕   ◥◣    "
                            " ▭▭▭▭━━━━━━▨▨▨▨ "
                            "                ",
            },
            (struct test) {
                .delta = { .msecs = 700 },
                .expected = "                "
                            " ═══            "
                            "    ◥◣          "
                            "      ◥◣        "
                            "      ▕ ◥◣      "
                            "      ▕   ◥◣    "
                            " ▭▭▭▭━━━━━━▨▨▨▨ "
                            "                ",
            },
            (struct test) {
                .delta = { .msecs = 900 },
                .expected = "                "
                            " ━══            "
                            "    ◥◣          "
                            "      ◥◣        "
                            "      ▕ ◥◣      "
                            "      ▕   ◥◣    "
                            " ▭▭▭▭━━━━━━▨▨▨▨ "
                            "                ",
            },
            (struct test) {
                .delta = { .msecs = 6000 },
                .expected = "                "
                            " ━══            "
                            "                "
                            "    ◢◤◢◤◢◤◢◤    "
                            "      ▕         "
                            "      ▕         "
                            " ▭▭▭▭━━━━━━▨▨▨▨ "
                            "                ",
            },
            (struct test) {
                .delta = { .msecs = 2100 },
                .expected = "                "
                            " ═══            "
                            "          ◢◤    "
                            "        ◢◤      "
                            "      ◢◤        "
                            "    ◢◤▕         "
                            " ▭▬▬▭━━━━━━▨▨▨▨ "
                            "                ",
            },
            (struct test) {
                .delta = { .msecs = 150 },
                .expected = "                "
                            " ═══            "
                            "          ◢◤    "
                            "        ◢◤      "
                            "      ◢◤        "
                            "    ◢◤▕         "
                            " ▭▬▬▭━━━━━━▨▨▨▨ "
                            "                ",
            },
            (struct test) {
                .delta = { .msecs = 150 },
                .expected = "                "
                            " ═══            "
                            "          ◢◤    "
                            "        ◢◤      "
                            "      ◢◤        "
                            "    ◢◤▕         "
                            " ▬▭▭▬━━━━━━▨▨▨▨ "
                            "                ",
            },
            (struct test) {
                .delta = { .msecs = 150 },
                .expected = "                "
                            " ═══            "
                            "          ◢◤    "
                            "        ◢◤      "
                            "      ◢◤        "
                            "    ◢◤▕         "
                            " ▬▭▭▬━━━━━━▨▨▨▨ "
                            "                ",
            },
            (struct test) {
                .delta = { .msecs = 150 },
                .expected = "                "
                            " ═══            "
                            "          ◢◤    "
                            "        ◢◤      "
                            "      ◢◤        "
                            "    ◢◤▕         "
                            " ▭▭▭▭━━━━━━▨▨▨▨ "
                            "                ",
            },
            (struct test) {
                .delta = { .msecs = 300 },
                .expected = "                "
                            " ━══            "
                            "                "
                            "    ◢◤◢◤◢◤◢◤    "
                            "      ▕         "
                            "      ▕         "
                            " ▭▭▭▭━━━━━━▨▨▨▨ "
                            "                ",
            },
            (struct test) {
                .delta = { .msecs = 300 },
                .expected = "                "
                            " ━══            "
                            "    ◥◣          "
                            "      ◥◣        "
                            "      ▕ ◥◣      "
                            "      ▕   ◥◣    "
                            " ▭▭▭▭━━━━━━▨▨▨▨ "
                            "                ",
            },
        };
        static size_t tests_len = sizeof(tests) / sizeof(struct test);

        for (size_t i = 0; i < tests_len; i++) {
            const struct test test = tests[i];

            tick_for(test.delta, &ccodoc, NULL);

            RENDER(&renderer, {
                struct drawing_ctx ctx = {
                    .origin = { .x = 1, .y = 1 }
                };
                ctx.current = ctx.origin;

                render_ccodoc(&renderer, &ctx, &ccodoc);
            });

            EXPECT_CANVAS(renderer.canvas->delegate.buffer, test.expected);
        }

        deinit_canvas(&canvas);
    }

    {
        printf("\n## timer\n");

        struct canvas_buffer canvas_buffer = { 0 };
        init_canvas_buffer(&canvas_buffer, (struct vec2d) { .x = 14 + 2, .y = 2 + 2 });

        struct canvas canvas = wrap_canvas_buffer(&canvas_buffer);

        struct renderer renderer = { .canvas = &canvas };

        struct timer timer = {
            .duration = duration_from_moment((struct moment) { .mins = 5 }),
        };

        static struct test {
            struct duration delta;
            const char* expected;
        } tests[] = {
            (struct test) {
                .delta = { .msecs = 0 },
                .expected = "                "
                            "     00ᴴ05ᴹ     "
                            " ────────────── "
                            "                ",
            },
            (struct test) {
                .delta = { .msecs = (long)30 * time_sec },
                .expected = "                "
                            "     00ᴴ05ᴹ     "
                            " ─────────────  "
                            "                ",
            },
            (struct test) {
                .delta = { .msecs = (long)30 * time_sec },
                .expected = "                "
                            "     00ᴴ04ᴹ     "
                            " ────────────   "
                            "                ",
            },
            (struct test) {
                .delta = { .msecs = (long)2 * time_min },
                .expected = "                "
                            "     00ᴴ02ᴹ     "
                            " ──────         "
                            "                ",
            },
            (struct test) {
                .delta = { .msecs = (long)1 * time_min + (long)39 * time_sec },
                .expected = "                "
                            "     00ᴴ01ᴹ     "
                            " ─              "
                            "                ",
            },
            (struct test) {
                .delta = { .msecs = 21000 },
                .expected = "                "
                            "     00ᴴ00ᴹ     "
                            "                "
                            "                ",
            },
        };

        static size_t tests_len = sizeof(tests) / sizeof(struct test);

        for (size_t i = 0; i < tests_len; i++) {
            const struct test test = tests[i];

            tick_for(test.delta, NULL, &timer);

            RENDER(&renderer, {
                struct drawing_ctx ctx = {
                    .origin = { .x = 1, .y = 1 }
                };
                ctx.current = ctx.origin;

                render_timer(&renderer, &ctx, &timer);
            });

            EXPECT_CANVAS(renderer.canvas->delegate.buffer, test.expected);
        }

        deinit_canvas(&canvas);
    }

    return EXIT_SUCCESS;
}

static void tick_for(const struct duration delta, struct ccodoc* const ccodoc, struct timer* const timer)
{
    static const struct duration min_delta = { .msecs = 100 };

    for (struct duration elapsed = { 0 }; elapsed.msecs < delta.msecs;) {
        const struct duration d = {
            .msecs = MIN(min_delta.msecs, delta.msecs - elapsed.msecs),
        };

        if (ccodoc != NULL) {
            tick_ccodoc(ccodoc, d);
        }
        if (timer != NULL) {
            tick_timer(timer, d);
        }

        elapsed.msecs += d.msecs;
    }
}

static void print_canvas(const struct canvas_buffer* const canvas)
{
    for (unsigned int h = 0; h < canvas->size.y; h++) {
        for (unsigned int w = 0; w < canvas->size.x; w++) {
            const unsigned int i = h * canvas->size.x + w;

            char s[5] = { 0 };
            uint32_t code = canvas->data[i].code;
            encode_char_utf8(s, code);
            printf("%s", s);
        }

        printf("\n");
    }
}

static int expect_canvas(const char* const file, const int line, const struct canvas_buffer* const actual, const char* const expected)
{
    print_canvas(actual);

    struct char_descriptor* const expected_chars = calloc((unsigned long)actual->size.x * actual->size.y, sizeof(struct char_descriptor));
    assert(expected_chars != NULL);

    decode_str_utf8(expected_chars, expected);

    for (unsigned int h = 0; h < actual->size.y; h++) {
        for (unsigned int w = 0; w < actual->size.x; w++) {
            const unsigned int i = h * actual->size.x + w;

            const struct char_descriptor expected_char = expected_chars[i];

            const uint32_t code = actual->data[i].code;
            if (code != expected_char.code) {
                char label[1 << 5] = { 0 };
                (void)snprintf(label, sizeof(label), "%d:%d", h, w);

                char actual_label[1 << 8] = { 0 };
                {
                    char s[5] = { 0 };
                    encode_char_utf8(s, code);

                    (void)snprintf(
                        actual_label, sizeof(actual_label),
                        "'%s'(%d)",
                        s, code
                    );
                }

                char expected_label[1 << 8] = { 0 };
                {
                    char s[5] = { 0 };
                    encode_char_utf8(s, expected_char.code);

                    (void)snprintf(
                        expected_label, sizeof(expected_label),
                        "'%s'(%d)",
                        s, expected_char.code
                    );
                }

                report_status(file, line, false, label, actual_label, expected_label);

                return EXIT_FAILURE;
            }
        }
    }

    free(expected_chars);

    return EXIT_SUCCESS;
}
