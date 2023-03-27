#include "math.h"

vector2d_t vec2d_add(const vector2d_t vector, const vector2d_t other)
{
    return (vector2d_t) {
        .x = vector.x + other.x,
        .y = vector.y + other.y,
    };
}
