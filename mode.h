#pragma once

#include "ccodoc.h"
#include "platform.h"
#include "renderer.h"
#include "time.h"

struct mode_ctx {
    struct sig_handler* sig_handler;
};

enum mode_type {
    mode_wabi,
    mode_sabi,
};

struct mode {
    bool ornamental;
    bool debug;

    struct ccodoc ccodoc;
    struct timer timer;

    struct {
        struct renderer renderer;
        struct {
            struct canvas value;
            struct canvas_curses delegate;
            struct canvas_proxy proxy;
        } canvas;
    } rendering;

    struct {
        const char* tsutsu_drip;
        const char* tsutsu_bump;
        const char* uguisu_call;
    } sound;
};

extern void init_mode(struct mode* mode);
extern void deinit_mode(struct mode* mode);

extern void run_mode_wabi(const struct mode_ctx* ctx, struct mode* mode);
extern void run_mode_sabi(const struct mode_ctx* ctx, struct mode* mode);
