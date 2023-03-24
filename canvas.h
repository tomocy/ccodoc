#pragma once

#include "engine.h"
#include <curses.h>

#define TEST_WINDOW_HEIGHT 10
#define TEST_WINDOW_WIDTH 20

typedef struct {
    // only for test purpose for now
    uint32_t test_window[TEST_WINDOW_HEIGHT * TEST_WINDOW_WIDTH];

    WINDOW* window;
} canvas;

extern void init_canvas(canvas* canvas, const context* ctx, ccodoc* ccodoc);
extern void deinit_canvas(canvas* canvas, ccodoc* ccodoc);
extern void draw_ccodoc(canvas* canvas, const context* ctx, const timer* timer, const ccodoc* ccodoc);
