#pragma once

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(min, max, x) MIN(MAX(x, min), max)

typedef struct {
    unsigned int x;
    unsigned int y;
} vector2d_t;
