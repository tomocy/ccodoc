#include "canvas.h"

#include "math.h"
#include <curses.h>
#include <locale.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

static canvas_buffer_t* serve_current_canvas_buffer(canvas_proxy_t* canvas);

drawing_context_t init_drawing_context(const context_t* ctx, point_t origin)
{
    return (drawing_context_t) {
        .app = ctx,
        .origin = origin,
        .current = origin,
    };
}

void wrap_drawing_lines(drawing_context_t* ctx, unsigned int n)
{
    ctx->current.y += n;
    ctx->current.x = ctx->origin.x;
}

canvas_t wrap_canvas_buffer(canvas_buffer_t* canvas)
{
    return (canvas_t) {
        .type = canvas_type_buffer,
        .delegate = { .buffer = canvas },
    };
}

canvas_t wrap_canvas_curses(canvas_curses_t* canvas)
{
    return (canvas_t) {
        .type = canvas_type_curses,
        .delegate = { .curses = canvas },
    };
}

canvas_t wrap_canvas_proxy(canvas_proxy_t* canvas)
{
    return (canvas_t) {
        .type = canvas_type_proxy,
        .delegate = { .proxy = canvas },
    };
}

static void deinit_canvas_buffer(canvas_buffer_t* canvas);
static void deinit_canvas_curses(canvas_curses_t* canvas);

// NOLINTNEXTLINE(misc-no-recursion)
void deinit_canvas(canvas_t* canvas)
{
    canvas_delegate_t* delegate = &canvas->delegate;

    switch (canvas->type) {
    case canvas_type_buffer:
        deinit_canvas_buffer(delegate->buffer);
        break;
    case canvas_type_curses:
        deinit_canvas_curses(delegate->curses);
        break;
    case canvas_type_proxy: {
        {
            canvas_t canvas = wrap_canvas_buffer(
                serve_current_canvas_buffer(delegate->proxy)
            );
            deinit_canvas(&canvas);
        }
        {
            canvas_t canvas = wrap_canvas_curses(delegate->proxy->underlying);
            deinit_canvas(&canvas);
        }
        break;
    }
    }
}

static void clear_canvas_buffer(canvas_buffer_t* canvas);
static void clear_canvas_curses(canvas_curses_t* canvas);

// NOLINTNEXTLINE(misc-no-recursion)
void clear_canvas(canvas_t* canvas)
{
    canvas_delegate_t* delegate = &canvas->delegate;

    switch (canvas->type) {
    case canvas_type_buffer:
        clear_canvas_buffer(delegate->buffer);
        break;
    case canvas_type_curses:
        clear_canvas_curses(delegate->curses);
        break;
    case canvas_type_proxy: {
        canvas_t canvas = wrap_canvas_buffer(
            serve_current_canvas_buffer(delegate->proxy)
        );
        clear_canvas(&canvas);
        break;
    }
    }
}

static void flush_canvas_curses(canvas_curses_t* canvas);
static void flush_canvas_proxy(canvas_proxy_t* canvas);

// NOLINTNEXTLINE(misc-no-recursion)
void flush_canvas(canvas_t* canvas)
{
    canvas_delegate_t* delegate = &canvas->delegate;

    switch (canvas->type) {
    case canvas_type_buffer:
        break;
    case canvas_type_curses:
        flush_canvas_curses(delegate->curses);
        break;
    case canvas_type_proxy: {
        flush_canvas_proxy(delegate->proxy);
        break;
    }
    }
}

static void use_drawing_attr_curses(canvas_curses_t* canvas, drawing_attr_t attr);

// NOLINTNEXTLINE(misc-no-recursion)
void use_drawing_attr(canvas_t* canvas, drawing_attr_t attr)
{
    canvas_delegate_t* delegate = &canvas->delegate;

    switch (canvas->type) {
    case canvas_type_buffer:
        break;
    case canvas_type_curses:
        use_drawing_attr_curses(delegate->curses, attr);
        break;
    case canvas_type_proxy: {
        {
            canvas_t canvas = wrap_canvas_buffer(
                serve_current_canvas_buffer(delegate->proxy)
            );
            use_drawing_attr(&canvas, attr);
        }
        {
            canvas_t canvas = wrap_canvas_curses(delegate->proxy->underlying);
            use_drawing_attr(&canvas, attr);
        }
        break;
    }
    }
}

static void clear_drawing_attr_curses(canvas_curses_t* canvas, drawing_attr_t attr);

// NOLINTNEXTLINE(misc-no-recursion)
void clear_drawing_attr(canvas_t* canvas, drawing_attr_t attr)
{
    canvas_delegate_t* delegate = &canvas->delegate;

    switch (canvas->type) {
    case canvas_type_buffer:
        break;
    case canvas_type_curses:
        clear_drawing_attr_curses(delegate->curses, attr);
        break;
    case canvas_type_proxy: {
        {
            canvas_t canvas = wrap_canvas_buffer(
                serve_current_canvas_buffer(delegate->proxy)
            );
            clear_drawing_attr(&canvas, attr);
        }
        {
            canvas_t canvas = wrap_canvas_curses(delegate->proxy->underlying);
            clear_drawing_attr(&canvas, attr);
        }
        break;
    }
    }
}

static void draw_curses(canvas_curses_t* canvas, unsigned int y, unsigned int x, const char* s);
static void draw_buffer(canvas_buffer_t* canvas, unsigned int y, unsigned int x, const char* s);

// NOLINTNEXTLINE(misc-no-recursion)
void draw(canvas_t* canvas, unsigned int y, unsigned int x, const char* s)
{
    canvas_delegate_t* delegate = &canvas->delegate;

    switch (canvas->type) {
    case canvas_type_buffer:
        draw_buffer(delegate->buffer, y, x, s);
        break;
    case canvas_type_curses:
        draw_curses(delegate->curses, y, x, s);
        break;
    case canvas_type_proxy: {
        canvas_t canvas = wrap_canvas_buffer(
            serve_current_canvas_buffer(delegate->proxy)
        );
        draw(&canvas, y, x, s);
        break;
    }
    }
}

static void drawfv_buffer(canvas_buffer_t* canvas, unsigned int y, unsigned int x, const char* format, va_list args);
static void drawfv_curses(canvas_curses_t* canvas, unsigned int y, unsigned int x, const char* format, va_list args);

// NOLINTNEXTLINE(misc-no-recursion)
static void drawfv(canvas_t* canvas, unsigned int y, unsigned int x, const char* format, va_list args)
{
    canvas_delegate_t* delegate = &canvas->delegate;

    switch (canvas->type) {
    case canvas_type_buffer:
        drawfv_buffer(delegate->buffer, y, x, format, args);
        break;
    case canvas_type_curses:
        drawfv_curses(delegate->curses, y, x, format, args);
        break;
    case canvas_type_proxy: {
        canvas_t canvas = wrap_canvas_buffer(
            serve_current_canvas_buffer(delegate->proxy)
        );
        drawfv(&canvas, y, x, format, args);
        break;
    }
    }
}

void drawf(canvas_t* canvas, unsigned int y, unsigned int x, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    drawfv(canvas, y, x, format, args);

    va_end(args);
}

static point_t get_canvas_size_curses(const canvas_curses_t* canvas);

// NOLINTNEXTLINE(misc-no-recursion)
point_t get_canvas_size(const canvas_t* canvas)
{
    const canvas_delegate_t* delegate = &canvas->delegate;

    switch (canvas->type) {
    case canvas_type_buffer:
        return delegate->buffer->size;
    case canvas_type_curses:
        return get_canvas_size_curses(delegate->curses);
    case canvas_type_proxy: {
        canvas_t canvas = wrap_canvas_curses(delegate->proxy->underlying);
        return get_canvas_size(&canvas);
    }
    }
}

// buffer

void init_canvas_buffer(canvas_buffer_t* canvas, point_t size)
{
    canvas->size = size;
    canvas->data = calloc((unsigned long)size.x * size.y, sizeof(uint32_t));
}

static void deinit_canvas_buffer(canvas_buffer_t* canvas)
{
    free(canvas->data);
    canvas->size.x = 0;
    canvas->size.y = 0;
}

static void clear_canvas_buffer(canvas_buffer_t* canvas)
{
    for (unsigned int h = 0; h < canvas->size.y; h++) {
        for (unsigned int w = 0; w < canvas->size.x; w++) {
            canvas->data[h * canvas->size.x + w] = ' ';
        }
    }
}

static void draw_buffer(canvas_buffer_t* canvas, unsigned int y, unsigned int x, const char* s)
{
    unsigned int n = 0;
    const char* c = s;

    while (*c) {
        char_descriptor_t desc = decode_char_utf8(c);
        canvas->data[y * canvas->size.x + x + n] = desc.code;

        n++;
        c += desc.len;
    }
}

static void drawfv_buffer(canvas_buffer_t* canvas, unsigned int y, unsigned int x, const char* format, va_list args)
{
    char s[1 << 8] = { 0 };

    (void)vsprintf(s, format, args);
    draw_buffer(canvas, y, x, s);
}

static bool canvas_equals_buffer(const canvas_buffer_t* canvas, const canvas_buffer_t* other)
{
    return memcmp(
               canvas->data, other->data,
               (unsigned long)canvas->size.x * canvas->size.y * sizeof(uint32_t)
           )
        == 0;
}

// curses

static void register_color(color_t color, short r, short g, short b, short supplement);
static short as_color_curses(color_t color);

void init_canvas_curses(canvas_curses_t* canvas, bool decorative)
{
    (void)setlocale(LC_ALL, "");

    canvas->window = initscr();
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

static void deinit_canvas_curses(canvas_curses_t* canvas)
{
    endwin();
    canvas->window = NULL;
}

static void clear_canvas_curses(canvas_curses_t* canvas)
{
    (void)canvas;
    clear();
}

static void flush_canvas_curses(canvas_curses_t* canvas)
{
    (void)canvas;
    refresh();
}

static unsigned int drawing_attr_flags(drawing_attr_t attr)
{
    unsigned int flags = 0;
    flags |= COLOR_PAIR(attr.color);
    flags |= attr.dim ? A_DIM : 0;
    flags |= attr.bold ? A_BOLD : 0;
    flags |= attr.underline ? A_UNDERLINE : 0;

    return flags;
}

static void use_drawing_attr_curses(canvas_curses_t* canvas, drawing_attr_t attr)
{
    (void)canvas;

    const unsigned int flags = drawing_attr_flags(attr);
    attron(flags);
}

static void clear_drawing_attr_curses(canvas_curses_t* canvas, drawing_attr_t attr)
{
    (void)canvas;

    const unsigned int flags = drawing_attr_flags(attr);
    attroff(flags);
}

static void draw_curses(canvas_curses_t* canvas, unsigned int y, unsigned int x, const char* s)
{
    mvwprintw(canvas->window, (int)y, (int)x, s);
}

static void drawfv_curses(canvas_curses_t* canvas, unsigned int y, unsigned int x, const char* format, va_list args)
{
    wmove(canvas->window, (int)y, (int)x);
    vw_printw(canvas->window, format, args);
}

static point_t get_canvas_size_curses(const canvas_curses_t* canvas)
{
    return (point_t) {
        .x = getmaxx(canvas->window),
        .y = getmaxy(canvas->window),
    };
}

static void register_color(color_t color, short r, short g, short b, short supplement)
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

static short as_color_curses(color_t color)
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

static canvas_buffer_t* serve_prev_canvas_buffer(canvas_proxy_t* canvas);
static void switch_canvas_buffer(canvas_proxy_t* canvas);

void init_canvas_proxy(canvas_proxy_t* canvas, canvas_curses_t* underlying)
{
    canvas->underlying = underlying;

    const point_t size = get_canvas_size_curses(canvas->underlying);

    for (int i = 0; i < CANVAS_PROXY_BUFFER_BUCKET_SIZE; i++) {
        init_canvas_buffer(&canvas->buffers[i], size);
    }
}

// NOLINTNEXTLINE(misc-no-recursion)
static void flush_canvas_proxy(canvas_proxy_t* canvas)
{
    const canvas_buffer_t* current = serve_current_canvas_buffer(canvas);
    const canvas_buffer_t* prev = serve_prev_canvas_buffer(canvas);

    if (canvas_equals_buffer(current, prev)) {
        return;
    }

    canvas_t underlying = wrap_canvas_curses(canvas->underlying);

    clear_canvas(&underlying);

    for (unsigned int y = 0; y < current->size.y; y++) {
        for (unsigned int x = 0; x < current->size.x; x++) {
            char c[5] = { 0 };
            unsigned int i = y * current->size.x + x;
            encode_char_utf8(c, current->data[i]);

            draw(&underlying, y, x, c);
        }
    }

    flush_canvas(&underlying);

    switch_canvas_buffer(canvas);
}

static canvas_buffer_t* serve_current_canvas_buffer(canvas_proxy_t* canvas)
{
    return &canvas->buffers[canvas->active_buffer_index];
}

static canvas_buffer_t* serve_prev_canvas_buffer(canvas_proxy_t* canvas)
{
    unsigned int i = (canvas->active_buffer_index + CANVAS_PROXY_BUFFER_BUCKET_SIZE - 1) % CANVAS_PROXY_BUFFER_BUCKET_SIZE;
    return &canvas->buffers[i];
}

static void switch_canvas_buffer(canvas_proxy_t* canvas)
{
    canvas->active_buffer_index = (canvas->active_buffer_index + 1) % CANVAS_PROXY_BUFFER_BUCKET_SIZE;
}
