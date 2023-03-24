#pragma once

#include "canvas.h"
#include "engine.h"

typedef struct {
    canvas canvas;
} renderer;

extern void init_renderer(renderer* renderer, const context* ctx, ccodoc* ccodoc);
extern void deinit_renderer(renderer* renderer, ccodoc* ccodoc);
extern void render_ccodoc(renderer* renderer, const context* ctx, const timer* timer, const ccodoc* ccodoc);
