#include "renderer.h"

#include "math.h"
#include "test.h"
#include "time.h"
#include <assert.h>
#include <stdio.h>

static void tick(const duration_t delta, ccodoc_t* ccodoc, tick_timer_t* timer);

static int expect_tick_render(const char* file, int line, duration_t delta, ccodoc_t* ccodoc, tick_timer_t* timer, renderer_t* renderer, const char* expected);
#define EXPECT_TICK_CCODOC(delta, ccodoc, timer, renderer, expected) EXPECT_PASS(expect_tick_render(__FILE__, __LINE__, delta, ccodoc, timer, renderer, expected))

int test_renderer(void)
{
    canvas_buffer_t canvas_buffer = { 0 };
    init_canvas_buffer(&canvas_buffer, (vec2d_t) { .x = 20, .y = 20 });

    canvas_t canvas = wrap_canvas_buffer(&canvas_buffer);

    renderer_t renderer = { .canvas = &canvas };

    ccodoc_t ccodoc = {
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

    {
        tick_timer_t timer = { .duration = duration_from_moment((moment_t) { .mins = 5 }) };

        {
            const duration_t delta = (duration_t) { .msecs = 0 };

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

            EXPECT_TICK_CCODOC(
                delta, &ccodoc, &timer, &renderer,
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

    deinit_canvas(&canvas);

    return EXIT_SUCCESS;
}

static void tick(const duration_t delta, ccodoc_t* const ccodoc, tick_timer_t* const timer)
{
    static const duration_t min_delta = { .msecs = 100 };

    for (duration_t elapsed = { 0 }; elapsed.msecs < delta.msecs;) {
        const duration_t d = {
            .msecs = MIN(delta.msecs - elapsed.msecs, min_delta.msecs),
        };

        tick_ccodoc(ccodoc, d);
        tick_timer(timer, d);

        elapsed.msecs += d.msecs;
    }
}

static void print_ccodoc(const canvas_buffer_t* canvas);
static int expect_canvas(const char* file, int line, const canvas_buffer_t* canvas, const char* expected);

static int expect_tick_render(
    const char* const file, const int line,
    const duration_t delta, ccodoc_t* const ccodoc, tick_timer_t* const timer, renderer_t* const renderer,
    const char* const expected
)
{
    tick(delta, ccodoc, timer);
    render(renderer, delta, ccodoc, timer);

    {
        const moment_t m = moment_from_duration(timer->ticker.elapsed, time_msec);
        printf("- elapsed: %um%us%ums =>\n", m.mins, m.secs, m.msecs);
    }

    print_ccodoc(renderer->canvas->delegate.buffer);

    return expect_canvas(file, line, renderer->canvas->delegate.buffer, expected);
}

static void print_ccodoc(const canvas_buffer_t* const canvas)
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

static int expect_canvas(const char* const file, const int line, const canvas_buffer_t* const canvas, const char* const expected)
{
    char_descriptor_t* const expected_chars = calloc((unsigned long)canvas->size.x * canvas->size.y, sizeof(char_descriptor_t));
    assert(expected_chars != NULL);

    decode_str_utf8(expected_chars, expected);

    for (unsigned int h = 0; h < canvas->size.y; h++) {
        for (unsigned int w = 0; w < canvas->size.x; w++) {
            const unsigned int i = h * canvas->size.x + w;

            const char_descriptor_t expected_char = expected_chars[i];

            const uint32_t code = canvas->data[i].code;
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
