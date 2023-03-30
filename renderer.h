#pragma once

#include "canvas.h"

#include "ccodoc.h"

typedef struct {
    canvas_t* canvas;

    bool ornamental;
    bool debug;
} renderer_t;

extern void deinit_renderer(renderer_t* renderer);

extern void render(
    renderer_t* renderer,
    duration_t delta,
    const ccodoc_t* ccodoc, const tick_timer_t* timer
);
