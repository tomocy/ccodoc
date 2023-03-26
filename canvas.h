#pragma once

#include "engine.h"
#include "string.h"
#include <curses.h>

typedef struct {
    unsigned int x;
    unsigned int y;
} point_t;

typedef enum {
    color_black,
    color_red,
    color_green,
    color_yellow,
    color_blue,
    color_grey,
    color_white,
} color_t;

typedef struct {
    color_t color;
    bool dim;
    bool bold;
} drawing_attr_t;

typedef struct {
    const context_t* app;

    point_t origin;
    point_t current;
} drawing_context_t;

extern drawing_context_t init_drawing_context(const context_t* ctx, point_t origin);
extern void wrap_drawing_lines(drawing_context_t* ctx, unsigned int n);

typedef struct {
    uint32_t code;
    drawing_attr_t attr;
} canvas_datum;

typedef struct {
    point_t size;
    canvas_datum* data;

    drawing_attr_t active_attr;
} canvas_buffer_t;

typedef struct {
    WINDOW* window;
} canvas_curses_t;

enum { CANVAS_PROXY_BUFFER_BUCKET_SIZE = 2 };

typedef struct {
    unsigned int active_buffer_index;
    canvas_buffer_t buffers[CANVAS_PROXY_BUFFER_BUCKET_SIZE];

    canvas_curses_t* underlying;
} canvas_proxy_t;

typedef enum {
    canvas_type_buffer,
    canvas_type_curses,
    canvas_type_proxy,
} canvas_type_t;

typedef union {
    canvas_buffer_t* buffer;
    canvas_curses_t* curses;
    canvas_proxy_t* proxy;
} canvas_delegate_t;

typedef struct {
    canvas_type_t type;
    canvas_delegate_t delegate;
} canvas_t;

extern canvas_t wrap_canvas_buffer(canvas_buffer_t* canvas);
extern canvas_t wrap_canvas_curses(canvas_curses_t* canvas);
extern canvas_t wrap_canvas_proxy(canvas_proxy_t* canvas);

extern void init_canvas_buffer(canvas_buffer_t* canvas, point_t size);
extern void init_canvas_curses(canvas_curses_t* canvas, bool decorative);
extern void init_canvas_proxy(canvas_proxy_t* canvas, canvas_curses_t* underlying);

extern void deinit_canvas(canvas_t* canvas);

extern void clear_canvas(canvas_t* canvas);
extern void flush_canvas(canvas_t* canvas);

extern void use_drawing_attr(canvas_t* canvas, drawing_attr_t attr);
extern void clear_drawing_attr(canvas_t* canvas, drawing_attr_t attr);

#define WITH_DRAWING_ATTR(in_canvas, attr, ...) \
    {                                           \
        canvas_t* canvas_ = (in_canvas);        \
        const drawing_attr_t attr_ = (attr);    \
        use_drawing_attr(canvas_, attr_);       \
        __VA_ARGS__;                            \
        clear_drawing_attr(canvas_, attr_);     \
    }

#define PREFER_DRAWING_WITH_ATTR(with_attr, canvas, attr, ...) \
    {                                                          \
        if (with_attr) {                                       \
            WITH_DRAWING_ATTR((canvas), (attr), __VA_ARGS__);  \
        } else {                                               \
            __VA_ARGS__                                        \
        }                                                      \
    }

extern void draw(canvas_t* canvas, unsigned int y, unsigned int x, const char* s);
extern void drawf(canvas_t* canvas, unsigned int y, unsigned int x, const char* format, ...);

extern point_t get_canvas_size(const canvas_t* canvas);
