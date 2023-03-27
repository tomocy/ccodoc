#pragma once

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(min, max, x) MIN(MAX(x, min), max)

typedef struct {
    unsigned int x;
    unsigned int y;
} vector2d_t;

extern vector2d_t vec2d_add(vector2d_t vector, vector2d_t other);
