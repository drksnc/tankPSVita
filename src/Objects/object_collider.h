#pragma once

class CObject;
class SDL_Rect;

class CObjectCollider
{
    public:
    CObjectCollider(CObject* base_object)
    {
        m_object = base_object;
    }

    enum CollisionSide
    {
        eCSTop = 1,
        eCSBottom = 2,
        eCSLeft = 4,
        eCSRight = 8,
        eCSNone = 16
    };

    void Update();

    CObject* m_object;

    private:
    bool IsIntersects(CObject* objectA, CObject* objectB, CollisionSide& side) ;
};