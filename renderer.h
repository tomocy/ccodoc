#pragma once

#include "canvas.h"

#include "engine.h"

typedef struct {
    canvas_t* canvas;
} renderer_t;

extern void init_renderer(renderer_t* renderer, canvas_t* canvas, ccodoc_t* ccodoc);
extern void deinit_renderer(renderer_t* renderer, ccodoc_t* ccodoc);
extern void render_ccodoc(renderer_t* renderer, const duration_t delta, const context_t* ctx, const tick_timer_t* timer, const ccodoc_t* ccodoc);
