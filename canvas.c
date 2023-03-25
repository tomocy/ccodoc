#include "canvas.h"

#include "math.h"
#include <curses.h>
#include <locale.h>
#include <stdarg.h>
#include <stdlib.h>

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

void init_canvas_buffer(canvas_buffer* canvas, point size)
{
    canvas->size = size;

    canvas->data = calloc((unsigned long)size.x * size.y, sizeof(uint32_t));
}

void deinit_canvas_buffer(canvas_buffer* canvas)
{
    free(canvas->data);
    canvas->size.x = 0;
    canvas->size.y = 0;
}

void clear_canvas_buffer(canvas_buffer* canvas)
{
    for (unsigned int h = 0; h < canvas->size.y; h++) {
        for (unsigned int w = 0; w < canvas->size.x; w++) {
            canvas->data[h * canvas->size.x + w] = ' ';
        }
    }
}

void draw_buffer(canvas_buffer* canvas, unsigned int y, unsigned int x, const char* s)
{
    unsigned int n = 0;
    const char* c = s;

    while (*c) {
        char_descriptor desc = decode_char_utf8(c);
        canvas->data[y * canvas->size.x + x + n] = desc.code;

        n++;
        c += desc.len;
    }
}

void drawfv_buffer(canvas_buffer* canvas, unsigned int y, unsigned int x, const char* format, va_list args)
{
    (void)canvas;
    (void)y;
    (void)x;
    (void)format;
    (void)args;
}

void drawf_buffer(canvas_buffer* canvas, unsigned int y, unsigned int x, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    drawfv_buffer(canvas, y, x, format, args);

    va_end(args);
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

void init_canvas_curses(canvas_curses* canvas, const context* ctx)
{
#if TESTING
    (void)ctx;
    init_canvas_buffer(&canvas->buffer, (point) { .x = 20, .y = 10 });
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

void deinit_canvas_curses(canvas_curses* canvas)
{
#if TESTING
    deinit_canvas_buffer(&canvas->buffer);
#else
    endwin();
    canvas->window = NULL;
#endif
}

void clear_canvas_curses(canvas_curses* canvas)
{
#if TESTING
    clear_canvas_buffer(&canvas->buffer);
#else
    (void)canvas;
    clear();
#endif
}

void flush_canvas_curses(canvas_curses* canvas)
{
#if TESTING
    (void)canvas;
#else
    (void)canvas;
    refresh();
#endif
}

void draw_curses(canvas_curses* canvas, unsigned int y, unsigned int x, const char* s)
{
#if TESTING
    draw_buffer(&canvas->buffer, y, x, s);
#else
    mvwprintw(canvas->window, (int)y, (int)x, s);
#endif
}

void drawf_curses(canvas_curses* canvas, unsigned int y, unsigned int x, const char* format, ...)
{
#if TESTING
    va_list args;
    va_start(args, format);

    drawfv_buffer(&canvas->buffer, y, x, format, args);

    va_end(args);
#else
    va_list args;
    va_start(args, format);

    wmove(canvas->window, (int)y, (int)x);
    vw_printw(canvas->window, format, args);

    va_end(args);
#endif
}

point drawing_window_size_curses(const canvas_curses* canvas)
{
#if TESTING
    return canvas->buffer.size;
#else
    return (point) {
        .x = getmaxx(canvas->window),
        .y = getmaxy(canvas->window),
    };
#endif
}
