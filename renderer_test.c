#include "renderer.h"

#include "ccodoc_test.h"
#include "math.h"
#include "time.h"
#include <assert.h>
#include <stdio.h>

static void tick(tick_timer_t* timer, ccodoc_t* ccodoc, const duration_t delta);
static void print_elapsed_time(const tick_timer_t* timer);
static int test_ccodoc(const canvas_buffer_t* actual, const char* expected);
static void draw_ccodoc(const canvas_buffer_t* actual);

int test_renderer(void)
{
    renderer_t renderer = { 0 };

    canvas_buffer_t canvas_buffer = { 0 };
    init_canvas_buffer(&canvas_buffer, (vector2d_t) { .x = 20, .y = 20 });

    canvas_t canvas = wrap_canvas_buffer(&canvas_buffer);

    ccodoc_t ccodoc = {
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

    init_renderer(&renderer, &canvas, &ccodoc);

    {
        printf("## canvas\n");

        context_t ctx = { 0 };

        tick_timer_t timer = { .duration = duration_from_moment((moment_t) { .mins = 5 }) };

        {
            const duration_t delta = (duration_t) { .msecs = 0 };

            render_ccodoc(&renderer, delta, &ctx, &timer, &ccodoc);

            print_elapsed_time(&timer);
            EXPECT_PASS_X(
                test_ccodoc(
                    renderer.canvas->delegate.buffer,
                    "                    "
                    "                    "
                    "                    "
                    "                    "
                    "                    "
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
                    "                    "
                    "                    "
                    "       00ᴴ05ᴹ       "
                    "   ──────────────   "
                    "                    "
                )
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 700 });

            tick(&timer, &ccodoc, delta);

            render_ccodoc(&renderer, delta, &ctx, &timer, &ccodoc);

            print_elapsed_time(&timer);
            EXPECT_PASS_X(
                test_ccodoc(
                    renderer.canvas->delegate.buffer,
                    "                    "
                    "                    "
                    "                    "
                    "                    "
                    "                    "
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
                    "                    "
                    "                    "
                    "       00ᴴ05ᴹ       "
                    "   ──────────────   "
                    "                    "
                )
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 700 });

            tick(&timer, &ccodoc, delta);

            render_ccodoc(&renderer, delta, &ctx, &timer, &ccodoc);

            print_elapsed_time(&timer);
            EXPECT_PASS_X(
                test_ccodoc(
                    renderer.canvas->delegate.buffer,
                    "                    "
                    "                    "
                    "                    "
                    "                    "
                    "                    "
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
                    "                    "
                    "                    "
                    "       00ᴴ05ᴹ       "
                    "   ──────────────   "
                    "                    "
                )
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 700 });

            tick(&timer, &ccodoc, delta);

            render_ccodoc(&renderer, delta, &ctx, &timer, &ccodoc);

            print_elapsed_time(&timer);
            EXPECT_PASS_X(
                test_ccodoc(
                    renderer.canvas->delegate.buffer,
                    "                    "
                    "                    "
                    "                    "
                    "                    "
                    "                    "
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
                    "                    "
                    "                    "
                    "       00ᴴ05ᴹ       "
                    "   ──────────────   "
                    "                    "
                )
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 900 });

            tick(&timer, &ccodoc, delta);

            render_ccodoc(&renderer, delta, &ctx, &timer, &ccodoc);

            print_elapsed_time(&timer);
            EXPECT_PASS_X(
                test_ccodoc(
                    renderer.canvas->delegate.buffer,
                    "                    "
                    "                    "
                    "                    "
                    "                    "
                    "                    "
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
                    "                    "
                    "                    "
                    "       00ᴴ05ᴹ       "
                    "   ──────────────   "
                    "                    "
                )
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 700 });

            tick(&timer, &ccodoc, delta);

            render_ccodoc(&renderer, delta, &ctx, &timer, &ccodoc);

            print_elapsed_time(&timer);
            EXPECT_PASS_X(
                test_ccodoc(
                    renderer.canvas->delegate.buffer,
                    "                    "
                    "                    "
                    "                    "
                    "                    "
                    "                    "
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
                    "                    "
                    "                    "
                    "       00ᴴ05ᴹ       "
                    "   ──────────────   "
                    "                    "
                )
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 700 });

            tick(&timer, &ccodoc, delta);

            render_ccodoc(&renderer, delta, &ctx, &timer, &ccodoc);

            print_elapsed_time(&timer);
            EXPECT_PASS_X(
                test_ccodoc(
                    renderer.canvas->delegate.buffer,
                    "                    "
                    "                    "
                    "                    "
                    "                    "
                    "                    "
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
                    "                    "
                    "                    "
                    "       00ᴴ05ᴹ       "
                    "   ──────────────   "
                    "                    "
                )
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 700 });

            tick(&timer, &ccodoc, delta);

            render_ccodoc(&renderer, delta, &ctx, &timer, &ccodoc);

            print_elapsed_time(&timer);
            EXPECT_PASS_X(
                test_ccodoc(
                    renderer.canvas->delegate.buffer,
                    "                    "
                    "                    "
                    "                    "
                    "                    "
                    "                    "
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
                    "                    "
                    "                    "
                    "       00ᴴ05ᴹ       "
                    "   ──────────────   "
                    "                    "
                )
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .secs = 6 });

            tick(&timer, &ccodoc, delta);

            render_ccodoc(&renderer, delta, &ctx, &timer, &ccodoc);

            print_elapsed_time(&timer);
            EXPECT_PASS_X(
                test_ccodoc(
                    renderer.canvas->delegate.buffer,
                    "                    "
                    "                    "
                    "                    "
                    "                    "
                    "                    "
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
                    "                    "
                    "                    "
                    "       00ᴴ05ᴹ       "
                    "   ──────────────   "
                    "                    "
                )
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .secs = 3 });

            tick(&timer, &ccodoc, delta);

            render_ccodoc(&renderer, delta, &ctx, &timer, &ccodoc);

            print_elapsed_time(&timer);
            EXPECT_PASS_X(
                test_ccodoc(
                    renderer.canvas->delegate.buffer,
                    "                    "
                    "                    "
                    "                    "
                    "                    "
                    "                    "
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
                    "                    "
                    "                    "
                    "       00ᴴ05ᴹ       "
                    "   ──────────────   "
                    "                    "
                )
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 300 });

            tick(&timer, &ccodoc, delta);

            render_ccodoc(&renderer, delta, &ctx, &timer, &ccodoc);

            print_elapsed_time(&timer);
            EXPECT_PASS_X(
                test_ccodoc(
                    renderer.canvas->delegate.buffer,
                    "                    "
                    "                    "
                    "                    "
                    "                    "
                    "                    "
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
                    "                    "
                    "                    "
                    "       00ᴴ05ᴹ       "
                    "   ──────────────   "
                    "                    "
                )
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 300 });
            tick(&timer, &ccodoc, delta);
            render_ccodoc(&renderer, delta, &ctx, &timer, &ccodoc);

            print_elapsed_time(&timer);
            EXPECT_PASS_X(
                test_ccodoc(
                    renderer.canvas->delegate.buffer,
                    "                    "
                    "                    "
                    "                    "
                    "                    "
                    "                    "
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
                    "                    "
                    "                    "
                    "       00ᴴ05ᴹ       "
                    "   ──────────────   "
                    "                    "
                )
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 300 });
            tick(&timer, &ccodoc, delta);
            render_ccodoc(&renderer, delta, &ctx, &timer, &ccodoc);

            print_elapsed_time(&timer);
            EXPECT_PASS_X(
                test_ccodoc(
                    renderer.canvas->delegate.buffer,
                    "                    "
                    "                    "
                    "                    "
                    "                    "
                    "                    "
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
                    "                    "
                    "                    "
                    "       00ᴴ05ᴹ       "
                    "   ──────────────   "
                    "                    "
                )
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 300 });
            tick(&timer, &ccodoc, delta);
            render_ccodoc(&renderer, delta, &ctx, &timer, &ccodoc);

            print_elapsed_time(&timer);
            EXPECT_PASS_X(
                test_ccodoc(
                    renderer.canvas->delegate.buffer,
                    "                    "
                    "                    "
                    "                    "
                    "                    "
                    "                    "
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
                    "                    "
                    "                    "
                    "       00ᴴ05ᴹ       "
                    "   ──────────────   "
                    "                    "
                )
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .secs = 14, .msecs = 700 });
            tick(&timer, &ccodoc, delta);
            render_ccodoc(&renderer, delta, &ctx, &timer, &ccodoc);

            print_elapsed_time(&timer);
            EXPECT_PASS_X(
                test_ccodoc(
                    renderer.canvas->delegate.buffer,
                    "                    "
                    "                    "
                    "                    "
                    "                    "
                    "                    "
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
                    "                    "
                    "                    "
                    "       00ᴴ05ᴹ       "
                    "   ─────────────    "
                    "                    "
                )
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .secs = 30 });
            tick(&timer, &ccodoc, delta);
            render_ccodoc(&renderer, delta, &ctx, &timer, &ccodoc);

            print_elapsed_time(&timer);
            EXPECT_PASS_X(
                test_ccodoc(
                    renderer.canvas->delegate.buffer,
                    "                    "
                    "                    "
                    "                    "
                    "                    "
                    "                    "
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
                    "                    "
                    "                    "
                    "       00ᴴ04ᴹ       "
                    "   ────────────     "
                    "                    "
                )
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .mins = 3, .secs = 30 });
            tick(&timer, &ccodoc, delta);
            render_ccodoc(&renderer, delta, &ctx, &timer, &ccodoc);

            print_elapsed_time(&timer);
            EXPECT_PASS_X(
                test_ccodoc(
                    renderer.canvas->delegate.buffer,
                    "                    "
                    "                    "
                    "                    "
                    "                    "
                    "                    "
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
                    "                    "
                    "                    "
                    "       00ᴴ01ᴹ       "
                    "   ──               "
                    "                    "
                )
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .secs = 30 });
            tick(&timer, &ccodoc, delta);
            render_ccodoc(&renderer, delta, &ctx, &timer, &ccodoc);

            print_elapsed_time(&timer);
            EXPECT_PASS_X(
                test_ccodoc(
                    renderer.canvas->delegate.buffer,
                    "                    "
                    "                    "
                    "                    "
                    "                    "
                    "                    "
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
                    "                    "
                    "                    "
                    "       00ᴴ00ᴹ       "
                    "                    "
                    "                    "
                )
            );
        }
    }

    deinit_renderer(&renderer, &ccodoc);

    return EXIT_SUCCESS;
}

static void tick(tick_timer_t* timer, ccodoc_t* ccodoc, const duration_t delta)
{
    static const duration_t min_delta = { .msecs = 100 };

    for (duration_t elapsed = { 0 }; elapsed.msecs < delta.msecs;) {
        const duration_t d = {
            .msecs = MIN(delta.msecs - elapsed.msecs, min_delta.msecs),
        };

        tick_timer(timer, d);
        tick_ccodoc(ccodoc, d);

        elapsed.msecs += d.msecs;
    }
}

static void print_elapsed_time(const tick_timer_t* timer)
{
    const moment_t m = moment_from_duration(timer->ticker.elapsed, time_msec);
    printf("- elapsed: %um%us%ums\n", m.mins, m.secs, m.msecs);
}

static int test_ccodoc(const canvas_buffer_t* actual, const char* expected)
{
    draw_ccodoc(actual);

    char_descriptor_t* expected_chars = calloc((unsigned long)actual->size.x * actual->size.y, sizeof(char_descriptor_t));
    assert(expected_chars != NULL);

    decode_str_utf8(expected_chars, expected);

    for (unsigned int h = 0; h < actual->size.y; h++) {
        for (unsigned int w = 0; w < actual->size.x; w++) {
            const unsigned int i = h * actual->size.x + w;

            const char_descriptor_t expected_char = expected_chars[i];

            const uint32_t code = actual->data[i].code;
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

static void draw_ccodoc(const canvas_buffer_t* canvas)
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
