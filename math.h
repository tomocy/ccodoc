#pragma once

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(min, max, x) MIN(MAX(x, min), max)

struct vec2d {
    unsigned int x;
    unsigned int y;
};

extern struct vec2d vec2d_add(struct vec2d vector, struct vec2d other);
