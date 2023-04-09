#include "memory.h"

#include <string.h>

bool mem_equals_n(const void* const mem, const void* const other, const unsigned int n)
{
    return memcmp(mem, other, n) == 0;
}
