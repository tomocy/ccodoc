#pragma once

#include "ccodoc.h"
#include "renderer.h"
#include "time.h"

typedef enum {
    mode_wabi,
    mode_sabi,
} ccodoc_mode_type_t;

typedef struct {
    bool ornamental;
    bool debug;

    ccodoc_t ccodoc;
    tick_timer_t timer;

    struct {
        renderer_t renderer;
        struct {
            canvas_t value;
            canvas_curses_t delegate;
            canvas_proxy_t proxy;
        } canvas;
    } rendering;

    struct {
        const char* tsutsu_drip;
        const char* tsutsu_bump;
        const char* uguisu_call;
    } sound;
} ccodoc_mode_t;

extern void init_mode(ccodoc_mode_t* mode);
extern void deinit_mode(ccodoc_mode_t* mode);

extern void run_mode_wabi(ccodoc_mode_t* mode);
extern void run_mode_sabi(ccodoc_mode_t* mode);
