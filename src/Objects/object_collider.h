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

    void Update();

    CObject* m_object;

    private:
    bool IsCollide(CObject* objectA, CObject* objectB);
};