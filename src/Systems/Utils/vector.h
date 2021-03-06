#pragma once

class Fvector
{
    public:
    int x = 0; int y = 0;

    Fvector& set(int _x, int _y);
    Fvector& sub(Fvector& vec);
    int distance_to(Fvector& vec);
    bool equals(Fvector& vec);
    int heuristic(Fvector& vec0, Fvector& vec1);
};