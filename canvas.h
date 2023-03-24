#pragma once

#include "engine.h"
#include "string.h"
#include <curses.h>

#define TEST_WINDOW_HEIGHT 10
#define TEST_WINDOW_WIDTH 20

typedef struct {
    // only for test purpose for now
    uint32_t test_window[TEST_WINDOW_HEIGHT * TEST_WINDOW_WIDTH];

    WINDOW* window;
} canvas;

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

#if TESTING
extern void draw(int y, int x, const char* s);
#else
#define draw(y, x, s) mvprintw((int)(y), (int)(x), (s))
#endif

#define drawf(y, x, format, ...) mvprintw((int)(y), (int)(x), (format), __VA_ARGS__)

extern void init_canvas(canvas* canvas, const context* ctx);
extern void deinit_canvas(canvas* canvas);
extern void clear_canvas(canvas* canvas);
extern void flush_canvas(canvas* canvas);

extern point drawing_window_size(const canvas* canvas);

extern drawing_context init_drawing_context(const context* ctx, point origin);
extern void wrap_drawing_lines(drawing_context* ctx, unsigned int n);
