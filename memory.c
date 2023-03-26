#include "memory.h"

#include <string.h>

bool mem_equals_n(void* mem, void* other, unsigned int n)
{
    return memcmp(mem, other, n) == 0;
}
