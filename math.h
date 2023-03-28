#pragma once

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(min, max, x) MIN(MAX(x, min), max)

typedef struct {
    unsigned int x;
    unsigned int y;
} vec2d_t;

extern vec2d_t vec2d_add(vec2d_t vector, vec2d_t other);
