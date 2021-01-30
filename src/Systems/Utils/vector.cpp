#include "vector.h"
#include <cmath>

Fvector& Fvector::set(int _x, int _y)
{
    x = _x; y = _y;
    return *this;
}

Fvector& Fvector::sub(Fvector& vec)
{
    x = abs(x - vec.x); y = abs(y - vec.y);
    return *this;
}

int Fvector::distance_to(Fvector& vec)
{
    return abs(x-vec.x) + abs(y-vec.y);
}

bool Fvector::equals(Fvector& vec)
{
    return x == vec.x && y == vec.y;
}

int Fvector::heuristic(Fvector& vec0, Fvector& vec1) 
{
    return abs(vec0.x - vec1.x) + abs(vec0.y - vec1.y);
}