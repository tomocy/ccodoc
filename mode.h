#pragma once

#include "ccodoc.h"
#include "renderer.h"
#include "time.h"

typedef struct {
    renderer_t renderer;

    struct {
        canvas_t value;
        canvas_curses_t delegate;
        canvas_proxy_t proxy;
    } canvas;
} rendering_ctx_t;

typedef struct {
    ccodoc_t ccodoc;

    rendering_ctx_t rendering_ctx;
} mode_wabi_t;

typedef struct {
    ccodoc_t ccodoc;
    tick_timer_t timer;

    rendering_ctx_t rendering_ctx;
} mode_sabi_t;

typedef enum {
    mode_wabi,
    mode_sabi,
} mode_type_t;

typedef struct {
    mode_type_t type;

    union {
        mode_wabi_t wabi;
        mode_sabi_t sabi;
    } delegate;
} mode_t;

typedef struct {
    bool ornamental;

    struct {
        const char* tsutsu_poured;
        const char* tsutsu_bumped;
    } sound;

    bool debug;
} mode_opt_t;

extern void init_mode_wabi(mode_wabi_t* wabi, const mode_opt_t opt);
extern void init_mode_sabi(mode_sabi_t* sabi, duration_t duration, const mode_opt_t opt);

extern void deinit_mode(mode_t* mode);

extern void run_mode(mode_t* mode);
