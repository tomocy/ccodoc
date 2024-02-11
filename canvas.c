#include "canvas.h"

#include "math.h"
#include "memory.h"
#include "string.h"
#include <curses.h>
#include <locale.h>
#include <stdarg.h>
#include <stdlib.h>

static void deinit_canvas_buffer(struct canvas_buffer* canvas);
static void deinit_canvas_curses(struct canvas_curses* canvas);

static void clear_canvas_buffer(struct canvas_buffer* canvas);
static void clear_canvas_curses(struct canvas_curses* canvas);

static void flush_canvas_curses(struct canvas_curses* canvas);
static void flush_canvas_proxy(struct canvas_proxy* canvas);

static void draw_buffer(struct canvas_buffer* canvas, struct vec2d point, struct drawing_attr attr, const char* s);
static void draw_curses(struct canvas_curses* canvas, struct vec2d point, struct drawing_attr attr, const char* s);

static void drawfv_buffer(struct canvas_buffer* canvas, struct vec2d point, struct drawing_attr attr, const char* format, va_list args);
static void drawfv_curses(struct canvas_curses* canvas, struct vec2d point, struct drawing_attr attr, const char* format, va_list args);

static struct vec2d get_canvas_size_curses(const struct canvas_curses* canvas);

static struct canvas_buffer* serve_current_canvas_buffer(struct canvas_proxy* canvas);

struct canvas wrap_canvas_buffer(struct canvas_buffer* const canvas)
{
    return (struct canvas) {
        .type = canvas_buffer,
        .delegate = { .buffer = canvas },
    };
}

struct canvas wrap_canvas_curses(struct canvas_curses* const canvas)
{
    return (struct canvas) {
        .type = canvas_curses,
        .delegate = { .curses = canvas },
    };
}

struct canvas wrap_canvas_proxy(struct canvas_proxy* const canvas)
{
    return (struct canvas) {
        .type = canvas_proxy,
        .delegate = { .proxy = canvas },
    };
}

// NOLINTNEXTLINE(misc-no-recursion)
void deinit_canvas(struct canvas* const canvas)
{
    union canvas_delegate* const delegate = &canvas->delegate;

    switch (canvas->type) {
    case canvas_buffer:
        deinit_canvas_buffer(delegate->buffer);
        break;
    case canvas_curses:
        deinit_canvas_curses(delegate->curses);
        break;
    case canvas_proxy: {
        {
            struct canvas canvas = wrap_canvas_buffer(
                serve_current_canvas_buffer(delegate->proxy)
            );
            deinit_canvas(&canvas);
        }
        {
            struct canvas canvas = wrap_canvas_curses(delegate->proxy->underlying);
            deinit_canvas(&canvas);
        }
        break;
    }
    }
}

// NOLINTNEXTLINE(misc-no-recursion)
void clear_canvas(struct canvas* const canvas)
{
    union canvas_delegate* const delegate = &canvas->delegate;

    switch (canvas->type) {
    case canvas_buffer:
        clear_canvas_buffer(delegate->buffer);
        break;
    case canvas_curses:
        clear_canvas_curses(delegate->curses);
        break;
    case canvas_proxy: {
        struct canvas canvas = wrap_canvas_buffer(
            serve_current_canvas_buffer(delegate->proxy)
        );
        clear_canvas(&canvas);
        break;
    }
    }
}

// NOLINTNEXTLINE(misc-no-recursion)
void flush_canvas(struct canvas* const canvas)
{
    union canvas_delegate* const delegate = &canvas->delegate;

    switch (canvas->type) {
    case canvas_buffer:
        break;
    case canvas_curses:
        flush_canvas_curses(delegate->curses);
        break;
    case canvas_proxy: {
        flush_canvas_proxy(delegate->proxy);
        break;
    }
    }
}

// NOLINTNEXTLINE(misc-no-recursion)
void draw(struct canvas* const canvas, const struct vec2d point, const struct drawing_attr attr, const char* const s)
{
    union canvas_delegate* const delegate = &canvas->delegate;

    switch (canvas->type) {
    case canvas_buffer:
        draw_buffer(delegate->buffer, point, attr, s);
        break;
    case canvas_curses:
        draw_curses(delegate->curses, point, attr, s);
        break;
    case canvas_proxy: {
        struct canvas canvas = wrap_canvas_buffer(
            serve_current_canvas_buffer(delegate->proxy)
        );
        draw(&canvas, point, attr, s);
        break;
    }
    }
}

// NOLINTNEXTLINE(misc-no-recursion)
void drawfv(struct canvas* const canvas, const struct vec2d point, const struct drawing_attr attr, const char* const format, va_list args)
{
    union canvas_delegate* const delegate = &canvas->delegate;

    switch (canvas->type) {
    case canvas_buffer:
        drawfv_buffer(delegate->buffer, point, attr, format, args);
        break;
    case canvas_curses:
        drawfv_curses(delegate->curses, point, attr, format, args);
        break;
    case canvas_proxy: {
        struct canvas canvas = wrap_canvas_buffer(
            serve_current_canvas_buffer(delegate->proxy)
        );
        drawfv(&canvas, point, attr, format, args);
        break;
    }
    }
}

void drawf(struct canvas* const canvas, const struct vec2d point, const struct drawing_attr attr, const char* const format, ...)
{
    va_list args = { 0 };
    va_start(args, format);

    drawfv(canvas, point, attr, format, args);

    va_end(args);
}

// NOLINTNEXTLINE(misc-no-recursion)
struct vec2d get_canvas_size(const struct canvas* const canvas)
{
    const union canvas_delegate* const delegate = &canvas->delegate;

    switch (canvas->type) {
    case canvas_buffer:
        return delegate->buffer->size;
    case canvas_curses:
        return get_canvas_size_curses(delegate->curses);
    case canvas_proxy: {
        struct canvas canvas = wrap_canvas_curses(delegate->proxy->underlying);
        return get_canvas_size(&canvas);
    }
    }
}

// buffer

void init_canvas_buffer(struct canvas_buffer* const canvas, const struct vec2d size)
{
    canvas->size = size;
    canvas->data = calloc(
        (long)size.x * size.y,
        sizeof(struct canvas_datum)
    );
}

static void deinit_canvas_buffer(struct canvas_buffer* const canvas)
{
    free(canvas->data);
    canvas->data = NULL;

    canvas->size.x = 0;
    canvas->size.y = 0;
}

static void clear_canvas_buffer(struct canvas_buffer* const canvas)
{
    for (unsigned int h = 0; h < canvas->size.y; h++) {
        for (unsigned int w = 0; w < canvas->size.x; w++) {
            canvas->data[h * canvas->size.x + w].code = ' ';
            canvas->data[h * canvas->size.x + w].attr = (struct drawing_attr) { 0 };
        }
    }
}

static void draw_buffer(struct canvas_buffer* const canvas, const struct vec2d point, const struct drawing_attr attr, const char* const s)
{
    unsigned int n = 0;
    const char* c = s;

    while (*c) {
        const struct char_descriptor desc = decode_char_utf8(c);

        const unsigned int i = point.y * canvas->size.x + point.x + n;
        struct canvas_datum* const datum = &canvas->data[i];
        datum->code = desc.code;
        datum->attr = attr;

        n++;
        c += desc.len;
    }
}

static void drawfv_buffer(
    struct canvas_buffer* const canvas,
    const struct vec2d point,
    const struct drawing_attr attr,
    const char* const format, va_list args
)
{
    char s[1 << 8] = { 0 };
    (void)vsprintf(s, format, args);
    draw_buffer(canvas, point, attr, s);
}

static unsigned int canvas_data_size_buffer(const struct canvas_buffer* const canvas)
{
    return (unsigned long)canvas->size.x * canvas->size.y * sizeof(struct canvas_datum);
}

static bool canvas_equals_buffer(const struct canvas_buffer* const canvas, const struct canvas_buffer* const other)
{
    return mem_equals_n(
        canvas->data, other->data,
        canvas_data_size_buffer(canvas)
    );
}

// curses

static void register_color_curses(enum color color, short r, short g, short b, short supplement);
static short as_color_curses(enum color color);

void init_canvas_curses(struct canvas_curses* const canvas)
{
    (void)setlocale(LC_ALL, "");

    canvas->window = initscr();

    noecho();
    curs_set(0);

    if (has_colors()) {
        start_color();

        static const short color_supplement = 10;

        register_color_curses(color_black, 0, 0, 0, 0);
        register_color_curses(color_red, 255, 123, 84, color_supplement);
        register_color_curses(color_green, 90, 190, 50, color_supplement);
        register_color_curses(color_yellow, 205, 180, 90, color_supplement);
        register_color_curses(color_blue, 0, 200, 220, color_supplement);
        register_color_curses(color_grey, 170, 160, 180, color_supplement);
        register_color_curses(color_white, 225, 230, 255, 0);

        bkgd(COLOR_PAIR(color_black));
    }
}

static void deinit_canvas_curses(struct canvas_curses* const canvas)
{
    endwin();
    canvas->window = NULL;
}

static void clear_canvas_curses(struct canvas_curses* const canvas)
{
    (void)canvas;
    clear();
}

static void flush_canvas_curses(struct canvas_curses* const canvas)
{
    (void)canvas;
    refresh();
}

static unsigned int drawing_attr_flags(const struct drawing_attr attr)
{
    unsigned int flags = 0;
    flags |= COLOR_PAIR(attr.color);
    flags |= attr.dim ? WA_DIM : 0;
    flags |= attr.bold ? WA_BOLD : 0;

    return flags;
}

#define WITH_DRAWING_ATTR_CURSES(attr, ...)                    \
    {                                                          \
        const unsigned int flags = drawing_attr_flags((attr)); \
        attron(flags);                                         \
        __VA_ARGS__;                                           \
        attroff(flags);                                        \
    }

static void draw_curses(struct canvas_curses* const canvas, const struct vec2d point, const struct drawing_attr attr, const char* const s)
{
    WITH_DRAWING_ATTR_CURSES(attr, {
        mvwprintw(canvas->window, (int)point.y, (int)point.x, "%s", s);
    });
}

static void drawfv_curses(struct canvas_curses* const canvas, const struct vec2d point, const struct drawing_attr attr, const char* const format, va_list args)
{
    WITH_DRAWING_ATTR_CURSES(attr, {
        wmove(canvas->window, (int)point.y, (int)point.x);
        vw_printw(canvas->window, format, args);
    });
}

static struct vec2d get_canvas_size_curses(const struct canvas_curses* const canvas)
{
    return (struct vec2d) {
        .x = getmaxx(canvas->window),
        .y = getmaxy(canvas->window),
    };
}

static void register_color_curses(const enum color color, const short r, const short g, const short b, const short supplement)
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

static short as_color_curses(const enum color color)
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

// proxy

static struct canvas_buffer* serve_prev_canvas_buffer(struct canvas_proxy* canvas);
static void switch_canvas_buffer(struct canvas_proxy* canvas);

void init_canvas_proxy(struct canvas_proxy* const canvas, struct canvas_curses* const underlying)
{
    canvas->underlying = underlying;

    const struct vec2d size = get_canvas_size_curses(canvas->underlying);

    for (int i = 0; i < CANVAS_PROXY_BUFFER_BUCKET_SIZE; i++) {
        init_canvas_buffer(&canvas->buffers[i], size);
    }
}

// NOLINTNEXTLINE(misc-no-recursion)
static void flush_canvas_proxy(struct canvas_proxy* const canvas)
{
    const struct canvas_buffer* const current = serve_current_canvas_buffer(canvas);
    const struct canvas_buffer* const prev = serve_prev_canvas_buffer(canvas);

    if (canvas_equals_buffer(current, prev)) {
        return;
    }

    struct canvas underlying = wrap_canvas_curses(canvas->underlying);

    clear_canvas(&underlying);

    for (unsigned int y = 0; y < current->size.y; y++) {
        for (unsigned int x = 0; x < current->size.x; x++) {
            unsigned int i = y * current->size.x + x;

            const struct canvas_datum* const datum = &current->data[i];

            char c[5] = { 0 };
            encode_char_utf8(c, datum->code);

            draw(&underlying, (struct vec2d) { .y = y, .x = x }, datum->attr, c);
        }
    }

    flush_canvas(&underlying);

    switch_canvas_buffer(canvas);
}

static struct canvas_buffer* serve_current_canvas_buffer(struct canvas_proxy* const canvas)
{
    return &canvas->buffers[canvas->active_buffer_index];
}

static struct canvas_buffer* serve_prev_canvas_buffer(struct canvas_proxy* const canvas)
{
    const unsigned int i = (canvas->active_buffer_index + CANVAS_PROXY_BUFFER_BUCKET_SIZE - 1) % CANVAS_PROXY_BUFFER_BUCKET_SIZE;
    return &canvas->buffers[i];
}

static void switch_canvas_buffer(struct canvas_proxy* const canvas)
{
    canvas->active_buffer_index = (canvas->active_buffer_index + 1) % CANVAS_PROXY_BUFFER_BUCKET_SIZE;
}

void wrap_drawing_lines(struct drawing_ctx* const ctx, const unsigned int n)
{
    ctx->current.y += n;
    ctx->current.x = ctx->origin.x;
}
