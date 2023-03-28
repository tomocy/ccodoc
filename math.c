#include "math.h"

vec2d_t vec2d_add(const vec2d_t vector, const vec2d_t other)
{
    return (vec2d_t) {
        .x = vector.x + other.x,
        .y = vector.y + other.y,
    };
}
