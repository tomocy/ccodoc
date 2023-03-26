#include "math.h"

vector2d_t vector2d_add(vector2d_t vector, vector2d_t other)
{
    return (vector2d_t) {
        .x = vector.x + other.x,
        .y = vector.y + other.y,
    };
}
