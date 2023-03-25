#pragma once

#include "engine.h"
#include "string.h"
#include <curses.h>

typedef struct {
    unsigned int x;
    unsigned int y;
} point;

typedef enum {
    color_black,
    color_red,
    color_green,
    color_yellow,
    color_blue,
    color_grey,
    color_white,
} color;

typedef struct {
    const context* app;

    point origin;
    point current;
} drawing_context;

extern drawing_context init_drawing_context(const context* ctx, point origin);
extern void wrap_drawing_lines(drawing_context* ctx, unsigned int n);

typedef struct {
    point size;
    uint32_t* data;
} canvas_buffer;

typedef struct {
    WINDOW* window;
} canvas_curses;

typedef enum {
    canvas_type_buffer,
    canvas_type_curses,
} canvas_type;

typedef struct {
    canvas_type type;

    union {
        canvas_buffer buffer;
        canvas_curses curses;
    } delegate;
} canvas;

extern void init_canvas(canvas* canvas, const context* ctx);
extern void deinit_canvas(canvas* canvas);
extern void clear_canvas(canvas* canvas);
extern void flush_canvas(canvas* canvas);

typedef struct {
    color color;
    bool dim;
    bool bold;
    bool underline;
} drawing_attr;

extern void use_drawing_attr(canvas* canvas, drawing_attr attr);
extern void clear_drawing_attr(canvas* canvas, drawing_attr attr);

#define WITH_DRAWING_ATTR(in_canvas, attr, ...) \
    {                                           \
        canvas* canvas_ = (in_canvas);          \
        const drawing_attr attr_ = (attr);      \
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

extern void draw(canvas* canvas, unsigned int y, unsigned int x, const char* s);
extern void drawf(canvas* canvas, unsigned int y, unsigned int x, const char* format, ...);

extern point get_canvas_size(const canvas* canvas);
