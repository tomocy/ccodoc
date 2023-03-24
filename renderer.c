#include "renderer.h"
#include "canvas.h"

void init_renderer(renderer* renderer, const context* ctx, ccodoc* ccodoc)
{
    init_canvas(&renderer->canvas, ctx, ccodoc);
}

void deinit_renderer(renderer* renderer, ccodoc* ccodoc)
{
    deinit_canvas(&renderer->canvas, ccodoc);
}

void render_ccodoc(renderer* renderer, const context* ctx, const timer* timer, const ccodoc* ccodoc)
{
    draw_ccodoc(&renderer->canvas, ctx, timer, ccodoc);
}
