#pragma once

#include "ccodoc.h"
#include "math.h"
#include "string.h"
#include <curses.h>

enum color {
    color_black,
    color_red,
    color_green,
    color_yellow,
    color_blue,
    color_grey,
    color_white,
};

struct drawing_attr {
    enum color color;
    bool dim;
    bool bold;
};

struct drawing_ctx {
    struct drawing_attr attr;

    struct vec2d origin;
    struct vec2d current;
};

struct canvas_datum {
    uint32_t code;
    struct drawing_attr attr;
};

struct canvas_buffer {
    struct vec2d size;
    struct canvas_datum* data;
};

struct canvas_curses {
    WINDOW* window;
};

enum { CANVAS_PROXY_BUFFER_BUCKET_SIZE = 2 };

struct canvas_proxy {
    unsigned int active_buffer_index;
    struct canvas_buffer buffers[CANVAS_PROXY_BUFFER_BUCKET_SIZE];

    struct canvas_curses* underlying;
};

enum canvas_type {
    canvas_buffer,
    canvas_curses,
    canvas_proxy,
};

union canvas_delegate {
    struct canvas_buffer* buffer;
    struct canvas_curses* curses;
    struct canvas_proxy* proxy;
};

struct canvas {
    enum canvas_type type;
    union canvas_delegate delegate;
};

extern struct canvas wrap_canvas_buffer(struct canvas_buffer* canvas);
extern struct canvas wrap_canvas_curses(struct canvas_curses* canvas);
extern struct canvas wrap_canvas_proxy(struct canvas_proxy* canvas);

extern void init_canvas_buffer(struct canvas_buffer* canvas, struct vec2d size);
extern void init_canvas_curses(struct canvas_curses* canvas);
extern void init_canvas_proxy(struct canvas_proxy* canvas, struct canvas_curses* underlying);

extern void deinit_canvas(struct canvas* canvas);

extern void clear_canvas(struct canvas* canvas);
extern void flush_canvas(struct canvas* canvas);

extern void draw(struct canvas* canvas, struct vec2d point, struct drawing_attr attr, const char* s);
extern void drawfv(struct canvas* canvas, struct vec2d point, struct drawing_attr attr, const char* format, va_list args);
extern void drawf(struct canvas* canvas, struct vec2d point, struct drawing_attr attr, const char* format, ...);

extern struct vec2d get_canvas_size(const struct canvas* canvas);

extern void wrap_drawing_lines(struct drawing_ctx* ctx, unsigned int n);
