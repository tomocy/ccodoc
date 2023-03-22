#include "ccodoc_vanilla.h"

#include <curses.h>

typedef struct {
    WINDOW* window;
} renderer_curses;

extern void init_renderer(renderer_curses* renderer, const context* ctx);
extern void deinit_renderer(renderer_curses* renderer);
extern void render_ccodoc(renderer_curses* renderer, const context* ctx, const timer* timer, const ccodoc* ccodoc);
