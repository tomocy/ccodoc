#pragma once

#include "canvas.h"

#include "ccodoc.h"

typedef struct {
    canvas_t* canvas;

    bool ornamental;
    bool debug;
} renderer_t;

extern void render_with(renderer_t* renderer, event_t render);

extern void render_ccodoc(renderer_t* renderer, drawing_ctx_t* ctx, const ccodoc_t* ccodoc);
extern void render_timer(renderer_t* renderer, drawing_ctx_t* ctx, const tick_timer_t* timer);
extern void render_debug_info(
    renderer_t* renderer,
    duration_t delta, const ccodoc_t* ccodoc, const tick_timer_t* timer
);
