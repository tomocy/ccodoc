#include "math.h"

struct vec2d vec2d_add(const struct vec2d vec, const struct vec2d other)
{
    return (struct vec2d) {
        .x = vec.x + other.x,
        .y = vec.y + other.y,
    };
}
