#pragma once

#include "engine.h"
#include "string.h"
#include <curses.h>

typedef struct {
    unsigned int x;
    unsigned int y;
} point;

typedef struct {
    const context* app;

    point origin;
    point current;
} drawing_context;

typedef enum {
    color_black = COLOR_BLACK,
    color_red = COLOR_RED,
    color_green = COLOR_GREEN,
    color_yellow = COLOR_YELLOW,
    color_blue = COLOR_BLUE,
    color_grey = COLOR_MAGENTA,
    color_white = COLOR_WHITE,
} color;

typedef struct {
    color color;
    bool dim;
    bool bold;
    bool underline;
} drawing_attr;

#define WITH_DRAWING_ATTR(attr, ...)            \
    {                                           \
        const drawing_attr x = (attr);          \
        int attrs = 0;                          \
        attrs |= COLOR_PAIR(x.color);           \
        attrs |= x.dim ? A_DIM : 0;             \
        attrs |= x.bold ? A_BOLD : 0;           \
        attrs |= x.underline ? A_UNDERLINE : 0; \
                                                \
        attron(attrs);                          \
        __VA_ARGS__;                            \
        attroff(attrs);                         \
    }

#define PREFER_DRAWING_WITH_ATTR(with_attr, attr, ...) \
    {                                                  \
        if (with_attr) {                               \
            WITH_DRAWING_ATTR((attr), __VA_ARGS__);    \
        } else {                                       \
            __VA_ARGS__                                \
        }                                              \
    }

extern drawing_context init_drawing_context(const context* ctx, point origin);
extern void wrap_drawing_lines(drawing_context* ctx, unsigned int n);

typedef struct {
    point size;

    uint32_t* data;
} canvas_buffer;

extern void init_canvas_buffer(canvas_buffer* canvas, point size);
extern void deinit_canvas_buffer(canvas_buffer* canvas);
extern void clear_canvas_buffer(canvas_buffer* canvas);

extern void draw_buffer(canvas_buffer* canvas, unsigned int y, unsigned int x, const char* s);
extern void drawf_buffer(canvas_buffer* canvas, unsigned int y, unsigned int x, const char* format, ...);

typedef struct {
    // only for test purpose for now
    canvas_buffer buffer;

    WINDOW* window;
} canvas;

extern void init_canvas(canvas* canvas, const context* ctx);
extern void deinit_canvas(canvas* canvas);
extern void clear_canvas(canvas* canvas);
extern void flush_canvas(canvas* canvas);

extern void draw(canvas* canvas, unsigned int y, unsigned int x, const char* s);
extern void drawf(canvas* canvas, unsigned int y, unsigned int x, const char* format, ...);

extern point drawing_window_size(const canvas* canvas);
