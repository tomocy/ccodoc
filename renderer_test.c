#include "renderer.h"

#include "ccodoc_test.h"

#include <assert.h>

static int test_ccodoc(const canvas_buffer* actual, const char* expected);

int test_renderer(void)
{
    renderer renderer = { 0 };

    context ctx = { 0 };

    timer timer = { .duration = duration_from_moment((moment) { .mins = 30 }) };

    ccodoc ccodoc = {
        .kakehi = {
            .release_water_ratio = 0.2f,
            .holding_water_timer = {
                .duration = { .msecs = 2100 },
            },
            .releasing_water_timer = {
                .duration = { .msecs = 900 },
            },
        },
        .tsutsu = {
            .water_capacity = 10,
            .releasing_water_timer = {
                .duration = { .msecs = 1200 },
            },
        },
        .hachi = {
            .releasing_water_timer = {
                .duration = { .msecs = 1000 },
            },
        },
    };

    init_renderer(&renderer, &ctx, &ccodoc);

    {
        printf("## canvas\n");

        {
            render_ccodoc(&renderer, &ctx, &timer, &ccodoc);

            EXPECT_PASS(
                "- initial\n",
                test_ccodoc(
                    &renderer.canvas.buffer,
                    "                    "
                    "                    "
                    "   ━══              "
                    "      ◥◣            "
                    "        ◥◣          "
                    "        ▕ ◥◣        "
                    "        ▕   ◥◣      "
                    "   ▭▭▭▭━━━━━━▨▨▨▨   "
                    "                    "
                    "                    "
                )
            );
        }

        {
            tick_ccodoc(&ccodoc, (duration) { .msecs = 700 });

            render_ccodoc(&renderer, &ctx, &timer, &ccodoc);

            EXPECT_PASS(
                "- tick (700ms)\n",
                test_ccodoc(
                    &renderer.canvas.buffer,
                    "                    "
                    "                    "
                    "   ═━═              "
                    "      ◥◣            "
                    "        ◥◣          "
                    "        ▕ ◥◣        "
                    "        ▕   ◥◣      "
                    "   ▭▭▭▭━━━━━━▨▨▨▨   "
                    "                    "
                    "                    "
                )
            );
        }

        {
            tick_ccodoc(&ccodoc, (duration) { .msecs = 700 });

            render_ccodoc(&renderer, &ctx, &timer, &ccodoc);

            EXPECT_PASS(
                "- tick (700ms)\n",
                test_ccodoc(
                    &renderer.canvas.buffer,
                    "                    "
                    "                    "
                    "   ══━              "
                    "      ◥◣            "
                    "        ◥◣          "
                    "        ▕ ◥◣        "
                    "        ▕   ◥◣      "
                    "   ▭▭▭▭━━━━━━▨▨▨▨   "
                    "                    "
                    "                    "
                )
            );
        }

        {
            tick_ccodoc(&ccodoc, (duration) { .msecs = 700 });

            render_ccodoc(&renderer, &ctx, &timer, &ccodoc);

            EXPECT_PASS(
                "- tick (700ms)\n",
                test_ccodoc(
                    &renderer.canvas.buffer,
                    "                    "
                    "                    "
                    "   ═══              "
                    "      ◥◣            "
                    "        ◥◣          "
                    "        ▕ ◥◣        "
                    "        ▕   ◥◣      "
                    "   ▭▭▭▭━━━━━━▨▨▨▨   "
                    "                    "
                    "                    "
                )
            );
        }

        {
            tick_ccodoc(&ccodoc, (duration) { .msecs = 900 });

            render_ccodoc(&renderer, &ctx, &timer, &ccodoc);

            EXPECT_PASS(
                "- tick (900ms)\n",
                test_ccodoc(
                    &renderer.canvas.buffer,
                    "                    "
                    "                    "
                    "   ━══              "
                    "      ◥◣            "
                    "        ◥◣          "
                    "        ▕ ◥◣        "
                    "        ▕   ◥◣      "
                    "   ▭▭▭▭━━━━━━▨▨▨▨   "
                    "                    "
                    "                    "
                )
            );
        }

        {
            tick_ccodoc(&ccodoc, (duration) { .msecs = 700 });

            render_ccodoc(&renderer, &ctx, &timer, &ccodoc);

            EXPECT_PASS(
                "- tick (700ms)\n",
                test_ccodoc(
                    &renderer.canvas.buffer,
                    "                    "
                    "                    "
                    "   ═━═              "
                    "      ◥◣            "
                    "        ◥◣          "
                    "        ▕ ◥◣        "
                    "        ▕   ◥◣      "
                    "   ▭▭▭▭━━━━━━▨▨▨▨   "
                    "                    "
                    "                    "
                )
            );
        }

        {
            tick_ccodoc(&ccodoc, (duration) { .msecs = 700 });

            render_ccodoc(&renderer, &ctx, &timer, &ccodoc);

            EXPECT_PASS(
                "- tick (700ms)\n",
                test_ccodoc(
                    &renderer.canvas.buffer,
                    "                    "
                    "                    "
                    "   ══━              "
                    "      ◥◣            "
                    "        ◥◣          "
                    "        ▕ ◥◣        "
                    "        ▕   ◥◣      "
                    "   ▭▭▭▭━━━━━━▨▨▨▨   "
                    "                    "
                    "                    "
                )
            );
        }

        {
            tick_ccodoc(&ccodoc, (duration) { .msecs = 700 });

            render_ccodoc(&renderer, &ctx, &timer, &ccodoc);

            EXPECT_PASS(
                "- tick (700ms)\n",
                test_ccodoc(
                    &renderer.canvas.buffer,
                    "                    "
                    "                    "
                    "   ═══              "
                    "      ◥◣            "
                    "        ◥◣          "
                    "        ▕ ◥◣        "
                    "        ▕   ◥◣      "
                    "   ▭▭▭▭━━━━━━▨▨▨▨   "
                    "                    "
                    "                    "
                )
            );
        }

        {
            tick_ccodoc(&ccodoc, (duration) { .msecs = 900 });
            tick_ccodoc(&ccodoc, (duration) { .msecs = 2100 });

            tick_ccodoc(&ccodoc, (duration) { .msecs = 900 });
            tick_ccodoc(&ccodoc, (duration) { .msecs = 2100 });

            render_ccodoc(&renderer, &ctx, &timer, &ccodoc);

            EXPECT_PASS(
                "- tick (6s)\n",
                test_ccodoc(
                    &renderer.canvas.buffer,
                    "                    "
                    "                    "
                    "   ═══              "
                    "                    "
                    "      ◢◤◢◤◢◤◢◤      "
                    "        ▕           "
                    "        ▕           "
                    "   ▭▭▭▭━━━━━━▨▨▨▨   "
                    "                    "
                    "                    "
                )
            );
        }

        {
            tick_ccodoc(&ccodoc, (duration) { .msecs = 900 });
            tick_ccodoc(&ccodoc, (duration) { .msecs = 2000 });
            tick_ccodoc(&ccodoc, (duration) { .msecs = 100 });

            render_ccodoc(&renderer, &ctx, &timer, &ccodoc);

            EXPECT_PASS(
                "- tick (3.1s)\n",
                test_ccodoc(
                    &renderer.canvas.buffer,
                    "                    "
                    "                    "
                    "   ═══              "
                    "            ◢◤      "
                    "          ◢◤        "
                    "        ◢◤          "
                    "      ◢◤▕           "
                    "   ▭▬▬▭━━━━━━▨▨▨▨   "
                    "                    "
                    "                    "
                )
            );
        }

        {
            tick_ccodoc(&ccodoc, (duration) { .msecs = 300 });

            render_ccodoc(&renderer, &ctx, &timer, &ccodoc);

            EXPECT_PASS(
                "- tick (300ms)\n",
                test_ccodoc(
                    &renderer.canvas.buffer,
                    "                    "
                    "                    "
                    "   ═══              "
                    "            ◢◤      "
                    "          ◢◤        "
                    "        ◢◤          "
                    "      ◢◤▕           "
                    "   ▬▭▭▬━━━━━━▨▨▨▨   "
                    "                    "
                    "                    "
                )
            );
        }

        {
            tick_ccodoc(&ccodoc, (duration) { .msecs = 300 });

            render_ccodoc(&renderer, &ctx, &timer, &ccodoc);

            EXPECT_PASS(
                "- tick (300ms)\n",
                test_ccodoc(
                    &renderer.canvas.buffer,
                    "                    "
                    "                    "
                    "   ═══              "
                    "            ◢◤      "
                    "          ◢◤        "
                    "        ◢◤          "
                    "      ◢◤▕           "
                    "   ▭▭▭▭━━━━━━▨▨▨▨   "
                    "                    "
                    "                    "
                )
            );
        }

        {
            tick_ccodoc(&ccodoc, (duration) { .msecs = 300 });

            render_ccodoc(&renderer, &ctx, &timer, &ccodoc);

            EXPECT_PASS(
                "- tick (300ms)\n",
                test_ccodoc(
                    &renderer.canvas.buffer,
                    "                    "
                    "                    "
                    "   ━══              "
                    "                    "
                    "      ◢◤◢◤◢◤◢◤      "
                    "        ▕           "
                    "        ▕           "
                    "   ▭▭▭▭━━━━━━▨▨▨▨   "
                    "                    "
                    "                    "
                )
            );
        }

        {
            tick_ccodoc(&ccodoc, (duration) { .msecs = 300 });

            render_ccodoc(&renderer, &ctx, &timer, &ccodoc);

            EXPECT_PASS(
                "- tick (300ms)\n",
                test_ccodoc(
                    &renderer.canvas.buffer,
                    "                    "
                    "                    "
                    "   ━══              "
                    "      ◥◣            "
                    "        ◥◣          "
                    "        ▕ ◥◣        "
                    "        ▕   ◥◣      "
                    "   ▭▭▭▭━━━━━━▨▨▨▨   "
                    "                    "
                    "                    "
                )
            );
        }
    }

    deinit_renderer(&renderer, &ccodoc);

    return EXIT_SUCCESS;
}

static int test_ccodoc(const canvas_buffer* actual, const char* expected)
{
    char_descriptor* expected_chars = calloc((unsigned long)actual->size.x * actual->size.y, sizeof(char_descriptor));
    assert(expected_chars != NULL);

    decode_str_utf8(expected_chars, expected);

    for (unsigned int h = 0; h < actual->size.y; h++) {
        for (unsigned int w = 0; w < actual->size.x; w++) {
            const unsigned int i = h * actual->size.x + w;

            char s[5] = { 0 };
            uint32_t code = actual->data[i];
            encode_char_utf8(s, code);
            printf("%s", s);
        }

        printf("\n");
    }

    for (unsigned int h = 0; h < actual->size.y; h++) {
        for (unsigned int w = 0; w < actual->size.x; w++) {
            const unsigned int i = h * actual->size.x + w;

            const char_descriptor expected_char = expected_chars[i];

            const uint32_t code = actual->data[i];
            if (code != expected_char.code) {
                char actual_str[5] = { 0 };
                encode_char_utf8(actual_str, code);

                char expected_str[5] = { 0 };
                encode_char_utf8(expected_str, expected_char.code);

                REPORTF_FAIL(
                    "%d:%d: actual '%s'(%d) != expected: '%s'(%d)\n",
                    h, w,
                    actual_str, code, expected_str, expected_char.code
                );

                return EXIT_FAILURE;
            }
        }
    }

    free(expected_chars);

    return EXIT_SUCCESS;
}
