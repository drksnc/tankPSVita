#include "vector.h"
#include <cmath>

void Fvector::set(int _x, int _y)
{
    x = _x; y = _y;
}

Fvector& Fvector::sub(Fvector& vec)
{
    x = abs(x - vec.x); y = abs(y - vec.y);
    return *this;
}

int Fvector::distance_to(Fvector& vec)
{
    return abs(x-vec.x + y-vec.y);
}