#include "math.h"

vec2d_t vec2d_add(const vec2d_t vec, const vec2d_t other)
{
    return (vec2d_t) {
        .x = vec.x + other.x,
        .y = vec.y + other.y,
    };
}
