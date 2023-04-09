#pragma once

#include "canvas.h"

#include "ccodoc.h"

typedef struct {
    canvas_t* canvas;

    bool ornamental;
} renderer_t;

#define RENDER(renderer, ...)               \
    {                                       \
        renderer_t* renderer_ = (renderer); \
        clear_canvas(renderer_->canvas);    \
        __VA_ARGS__;                        \
        flush_canvas(renderer_->canvas);    \
    }

extern void render_ccodoc(renderer_t* renderer, drawing_ctx_t* ctx, const ccodoc_t* ccodoc);
extern void render_timer(renderer_t* renderer, drawing_ctx_t* ctx, const tick_timer_t* timer);
extern void render_debug_info(
    renderer_t* renderer,
    duration_t delta, const ccodoc_t* ccodoc, const tick_timer_t* timer
);
