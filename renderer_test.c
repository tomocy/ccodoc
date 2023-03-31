#include "renderer.h"

#include "ccodoc_test.h"
#include "math.h"
#include "time.h"
#include <assert.h>
#include <stdio.h>

static void tick(tick_timer_t* timer, ccodoc_t* ccodoc, const duration_t delta);
static void print_elapsed_time(const tick_timer_t* timer);
static int test_rendering(const canvas_buffer_t* actual, const char* expected);
static void print_ccodoc(const canvas_buffer_t* canvas);

#define EXPECT_EQUAL_CCODOC(renderer, delta, ccodoc, timer, expected)                \
    {                                                                                \
        renderer_t* const renderer_ = (renderer);                                    \
        const duration_t delta_ = (delta);                                           \
        ccodoc_t* const ccodoc_ = (ccodoc);                                          \
        tick_timer_t* const timer_ = (timer);                                        \
                                                                                     \
        tick(timer_, ccodoc_, delta_);                                               \
        render(renderer_, delta_, ccodoc_, timer_);                                  \
                                                                                     \
        print_elapsed_time(timer_);                                                  \
        print_ccodoc(renderer_->canvas->delegate.buffer);                            \
        EXPECT_PASS(test_rendering(renderer_->canvas->delegate.buffer, (expected))); \
    }

int test_renderer(void)
{
    canvas_buffer_t canvas_buffer = { 0 };
    init_canvas_buffer(&canvas_buffer, (vec2d_t) { .x = 20, .y = 20 });

    canvas_t canvas = wrap_canvas_buffer(&canvas_buffer);

    renderer_t renderer = { .canvas = &canvas };

    ccodoc_t ccodoc = {
        .kakehi = {
            .release_water_ratio = 0.2f,
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

    {
        printf("## canvas\n");

        tick_timer_t timer = { .duration = duration_from_moment((moment_t) { .mins = 5 }) };

        {
            const duration_t delta = (duration_t) { .msecs = 0 };

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
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
            );
        }

        {
            const duration_t delta = (duration_t) { .msecs = 350 };

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
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
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 350 });

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
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
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 350 });

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
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
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 350 });

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
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
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 350 });

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
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
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 350 });

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
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
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 450 });

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
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
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 450 });

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
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
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 700 });

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
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
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 700 });

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
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
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 700 });

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
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
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 900 });

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
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
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .secs = 6 });

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
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
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 2100 });

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
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
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 150 });

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
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
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 150 });

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
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
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 150 });

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
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
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 150 });

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
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
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 300 });

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
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
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 300 });

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
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
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .secs = 13, .msecs = 700 });

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "   ══━              "
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
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 100 });

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
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
                "   ─────────────    "
                "                    "
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 1200 });

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
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
                "   ─────────────    "
                "                    "
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .mins = 3 });

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
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
                "       00ᴴ02ᴹ       "
                "   ─────            "
                "                    "
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .mins = 1, .secs = 28, .msecs = 700 });

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "                    "
                "   ══━              "
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
                "   ─                "
                "                    "
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 100 });

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
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
                "       00ᴴ01ᴹ       "
                "   ─                "
                "                    "
            );
        }

        {
            const duration_t delta = duration_from_moment((moment_t) { .msecs = 1200 });

            EXPECT_EQUAL_CCODOC(
                &renderer, delta, &ccodoc, &timer,
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
                "       00ᴴ00ᴹ       "
                "                    "
                "                    "
            );
        }
    }

    deinit_renderer(&renderer);

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

static int test_rendering(const canvas_buffer_t* actual, const char* expected)
{
    char_descriptor_t* const expected_chars = calloc((unsigned long)actual->size.x * actual->size.y, sizeof(char_descriptor_t));
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

static void print_ccodoc(const canvas_buffer_t* canvas)
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
