#pragma once

#include "canvas.h"

#include "ccodoc.h"

typedef struct {
    canvas_t* canvas;

    bool ornamental;

    struct {
        const char* tsutsu_poured;
        const char* tsutsu_bumped;
    } sound;

    bool debug;
} renderer_t;

extern void init_renderer(renderer_t* renderer, canvas_t* canvas, ccodoc_t* ccodoc);
extern void deinit_renderer(renderer_t* renderer, ccodoc_t* ccodoc);

extern void render(
    renderer_t* renderer,
    duration_t delta,
    const ccodoc_t* ccodoc, const tick_timer_t* timer
);
