#pragma once

#include "engine.h"
#include <curses.h>

typedef struct {
    WINDOW* window;
} canvas;

extern void init_canvas(canvas* canvas, const context* ctx, ccodoc* ccodoc);
extern void deinit_canvas(canvas* canvas, ccodoc* ccodoc);
extern void draw_ccodoc(canvas* canvas, const context* ctx, const timer* timer, const ccodoc* ccodoc);
