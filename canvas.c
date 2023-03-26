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

static void deinit_canvas_buffer(canvas_buffer* canvas);
static void deinit_canvas_curses(canvas_curses* canvas);

void deinit_canvas(canvas* canvas)
{
    canvas_delegate* delegate = &canvas->delegate;

    switch (canvas->type) {
    case canvas_type_buffer:
        deinit_canvas_buffer(&delegate->buffer);
        break;
    case canvas_type_curses:
        deinit_canvas_curses(&delegate->curses);
        break;
    }
}

static void clear_canvas_buffer(canvas_buffer* canvas);
static void clear_canvas_curses(canvas_curses* canvas);

void clear_canvas(canvas* canvas)
{
    canvas_delegate* delegate = &canvas->delegate;

    switch (canvas->type) {
    case canvas_type_buffer:
        clear_canvas_buffer(&delegate->buffer);
        break;
    case canvas_type_curses:
        clear_canvas_curses(&delegate->curses);
        break;
    }
}

static void flush_canvas_curses(canvas_curses* canvas);

void flush_canvas(canvas* canvas)
{
    canvas_delegate* delegate = &canvas->delegate;

    switch (canvas->type) {
    case canvas_type_buffer:
        break;
    case canvas_type_curses:
        flush_canvas_curses(&delegate->curses);
        break;
    }
}

static void use_drawing_attr_curses(canvas_curses* canvas, drawing_attr attr);

void use_drawing_attr(canvas* canvas, drawing_attr attr)
{
    canvas_delegate* delegate = &canvas->delegate;

    switch (canvas->type) {
    case canvas_type_buffer:
        break;
    case canvas_type_curses:
        use_drawing_attr_curses(&delegate->curses, attr);
        break;
    }
}

static void clear_drawing_attr_curses(canvas_curses* canvas, drawing_attr attr);

void clear_drawing_attr(canvas* canvas, drawing_attr attr)
{
    canvas_delegate* delegate = &canvas->delegate;

    switch (canvas->type) {
    case canvas_type_buffer:
        break;
    case canvas_type_curses:
        clear_drawing_attr_curses(&delegate->curses, attr);
        break;
    }
}

static void draw_curses(canvas_curses* canvas, unsigned int y, unsigned int x, const char* s);
static void draw_buffer(canvas_buffer* canvas, unsigned int y, unsigned int x, const char* s);

void draw(canvas* canvas, unsigned int y, unsigned int x, const char* s)
{
    canvas_delegate* delegate = &canvas->delegate;

    switch (canvas->type) {
    case canvas_type_buffer:
        draw_buffer(&delegate->buffer, y, x, s);
        break;
    case canvas_type_curses:
        draw_curses(&delegate->curses, y, x, s);
        break;
    }
}

static void drawfv_buffer(canvas_buffer* canvas, unsigned int y, unsigned int x, const char* format, va_list args);
static void drawfv_curses(canvas_curses* canvas, unsigned int y, unsigned int x, const char* format, va_list args);

void drawf(canvas* canvas, unsigned int y, unsigned int x, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    canvas_delegate* delegate = &canvas->delegate;

    switch (canvas->type) {
    case canvas_type_buffer:
        drawfv_buffer(&delegate->buffer, y, x, format, args);
        break;
    case canvas_type_curses:
        drawfv_curses(&delegate->curses, y, x, format, args);
        break;
    }

    va_end(args);
}

static point get_canvas_size_curses(const canvas_curses* canvas);

point get_canvas_size(const canvas* canvas)
{
    const canvas_delegate* delegate = &canvas->delegate;

    switch (canvas->type) {
    case canvas_type_buffer:
        return delegate->buffer.size;
    case canvas_type_curses:
        return get_canvas_size_curses(&delegate->curses);
    }
}

// buffer

void init_canvas_buffer(canvas* canvas, point size)
{
    canvas->type = canvas_type_buffer;

    canvas_buffer* delegate = &canvas->delegate.buffer;

    delegate->size = size;
    delegate->data = calloc((unsigned long)size.x * size.y, sizeof(uint32_t));
}

static void deinit_canvas_buffer(canvas_buffer* canvas)
{
    free(canvas->data);
    canvas->size.x = 0;
    canvas->size.y = 0;
}

static void clear_canvas_buffer(canvas_buffer* canvas)
{
    for (unsigned int h = 0; h < canvas->size.y; h++) {
        for (unsigned int w = 0; w < canvas->size.x; w++) {
            canvas->data[h * canvas->size.x + w] = ' ';
        }
    }
}

static void draw_buffer(canvas_buffer* canvas, unsigned int y, unsigned int x, const char* s)
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

static void drawfv_buffer(canvas_buffer* canvas, unsigned int y, unsigned int x, const char* format, va_list args)
{
    char s[1 << 8] = { 0 };

    (void)vsprintf(s, format, args);
    draw_buffer(canvas, y, x, s);
}

// curses

static void register_color(color color, short r, short g, short b, short supplement);
static short as_color_curses(color color);

void init_canvas_curses(canvas* canvas, bool decorative)
{
    canvas->type = canvas_type_curses;

    canvas_curses* delegate = &canvas->delegate.curses;

    (void)setlocale(LC_ALL, "");

    delegate->window = initscr();
    noecho();
    curs_set(0);

    if (!decorative) {
        return;
    }

    if (has_colors()) {
        start_color();

        static const short color_supplement = 10;

        register_color(color_black, 0, 0, 0, 0);
        register_color(color_red, 255, 123, 84, color_supplement);
        register_color(color_green, 90, 190, 50, color_supplement);
        register_color(color_yellow, 205, 180, 90, color_supplement);
        register_color(color_blue, 0, 200, 220, color_supplement);
        register_color(color_grey, 170, 160, 180, color_supplement);
        register_color(color_white, 225, 230, 255, 0);

        bkgd(COLOR_PAIR(color_black));
    }
}

static void deinit_canvas_curses(canvas_curses* canvas)
{
    endwin();
    canvas->window = NULL;
}

static void clear_canvas_curses(canvas_curses* canvas)
{
    (void)canvas;
    clear();
}

static void flush_canvas_curses(canvas_curses* canvas)
{
    (void)canvas;
    refresh();
}

static unsigned int drawing_attr_flags(drawing_attr attr)
{
    unsigned int flags = 0;
    flags |= COLOR_PAIR(attr.color);
    flags |= attr.dim ? A_DIM : 0;
    flags |= attr.bold ? A_BOLD : 0;
    flags |= attr.underline ? A_UNDERLINE : 0;

    return flags;
}

static void use_drawing_attr_curses(canvas_curses* canvas, drawing_attr attr)
{
    (void)canvas;

    const unsigned int flags = drawing_attr_flags(attr);
    attron(flags);
}

static void clear_drawing_attr_curses(canvas_curses* canvas, drawing_attr attr)
{
    (void)canvas;

    const unsigned int flags = drawing_attr_flags(attr);
    attroff(flags);
}

static void draw_curses(canvas_curses* canvas, unsigned int y, unsigned int x, const char* s)
{
    mvwprintw(canvas->window, (int)y, (int)x, s);
}

static void drawfv_curses(canvas_curses* canvas, unsigned int y, unsigned int x, const char* format, va_list args)
{
    wmove(canvas->window, (int)y, (int)x);
    vw_printw(canvas->window, format, args);
}

static point get_canvas_size_curses(const canvas_curses* canvas)
{
    return (point) {
        .x = getmaxx(canvas->window),
        .y = getmaxy(canvas->window),
    };
}

static void register_color(color color, short r, short g, short b, short supplement)
{
    static const float factor = 1000.0f / 255;

    const short curses_color = as_color_curses(color);

    if (can_change_color()) {
        init_color(
            curses_color,
            CLAMP(0, 1000, (r + 1 * supplement) * factor),
            CLAMP(0, 1000, (g + 1 * supplement) * factor),
            CLAMP(0, 1000, (b + 1 * supplement) * factor)
        );
    }

    init_pair(color, curses_color, as_color_curses(color_black));
}

static short as_color_curses(color color)
{
    switch (color) {
    case color_black:
        return COLOR_BLACK;
    case color_red:
        return COLOR_RED;
    case color_green:
        return COLOR_GREEN;
    case color_yellow:
        return COLOR_YELLOW;
    case color_blue:
        return COLOR_BLUE;
    case color_grey:
        return COLOR_MAGENTA;
    case color_white:
        return COLOR_WHITE;
    }
}
