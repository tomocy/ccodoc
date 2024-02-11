#pragma once

#include "canvas.h"
#include "ccodoc.h"

struct renderer {
    struct canvas* canvas;

    bool ornamental;
};

#define RENDER(r, ...)                    \
    {                                     \
        struct renderer* renderer_ = (r); \
        clear_canvas(renderer_->canvas);  \
        __VA_ARGS__;                      \
        flush_canvas(renderer_->canvas);  \
    }

extern void render_ccodoc(struct renderer* renderer, struct drawing_ctx* ctx, const struct ccodoc* ccodoc);
extern void render_timer(struct renderer* renderer, struct drawing_ctx* ctx, const struct timer* timer);
extern void render_debug_info(
    struct renderer* renderer,
    struct duration delta, const struct ccodoc* ccodoc, const struct timer* timer
);
