#include "canvas.h"

#include "math.h"
#include <curses.h>
#include <locale.h>

#if TESTING
static canvas* test_canvas = NULL;

void draw(int y, int x, const char* s)
{
    size_t i = 0;
    const char* c = s;

    while (*c) {
        char_descriptor desc = decode_char_utf8(c);
        size_t p = y * TEST_WINDOW_WIDTH + x + i;
        test_canvas->test_window[p] = desc.code;

        i++;
        c += desc.len;
    }
}
#endif

#if !TESTING
static void set_color(color color, short r, short g, short b, short supplement);
#endif

void init_canvas(canvas* canvas, const context* ctx)
{
#if TESTING
    (void)ctx;

    test_canvas = canvas;
#else
    (void)setlocale(LC_ALL, "");

    canvas->window = initscr();
    noecho();
    curs_set(0);

    if (!ctx->decorative) {
        return;
    }

    if (has_colors()) {
        start_color();

        static const short color_supplement = 10;

        set_color(color_black, 0, 0, 0, 0);
        set_color(color_red, 255, 123, 84, color_supplement);
        set_color(color_green, 90, 190, 50, color_supplement);
        set_color(color_yellow, 205, 180, 90, color_supplement);
        set_color(color_blue, 0, 200, 220, color_supplement);
        set_color(color_grey, 170, 160, 180, color_supplement);
        set_color(color_white, 225, 230, 255, 0);

        bkgd(COLOR_PAIR(color_black));
    }
#endif
}

void deinit_canvas(canvas* canvas)
{
#if TESTING
    (void)canvas;

    test_canvas = NULL;
#else
    endwin();
    canvas->window = NULL;
#endif
}

void clear_canvas(canvas* canvas)
{
#if TESTING
    for (int h = 0; h < TEST_WINDOW_HEIGHT; h++) {
        for (int w = 0; w < TEST_WINDOW_WIDTH; w++) {
            canvas->test_window[h * TEST_WINDOW_WIDTH + w] = ' ';
        }
    }
#else
    (void)canvas;
    clear();
#endif
}

void flush_canvas(canvas* canvas)
{
#if TESTING
    (void)canvas;
#else
    (void)canvas;
    refresh();
#endif
}

point drawing_window_size(const canvas* canvas)
{
#if TESTING
    (void)canvas;
    return (point) {
        .x = TEST_WINDOW_WIDTH,
        .y = TEST_WINDOW_HEIGHT,
    };
#else
    return (point) {
        .x = getmaxx(canvas->window),
        .y = getmaxy(canvas->window),
    };
#endif
}

drawing_context init_drawing_context(const context* ctx, point origin)
{
    return (drawing_context) {
        .app = ctx,
        .origin = origin,
        .current = origin,
    };
}

void wrap_drawing_lines(drawing_context* ctx, unsigned int n)
{
    ctx->current.y += n;
    ctx->current.x = ctx->origin.x;
}

#if !TESTING
static void set_color(color color, short r, short g, short b, short supplement)
{
    static const float factor = 1000.0f / 255;

    if (can_change_color()) {
        init_color(
            color,
            CLAMP(0, 1000, (r + 1 * supplement) * factor),
            CLAMP(0, 1000, (g + 1 * supplement) * factor),
            CLAMP(0, 1000, (b + 1 * supplement) * factor)
        );
    }

    init_pair(color, color, color_black);
}
#endif
