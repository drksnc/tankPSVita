#include "object_collider.h"
#include "Systems/Level/level.h"
#include "Systems/Render/render.h"
#include "object.h"
#include "actor.h"
#include "globals.h"
#include "Systems/engine.h"
#include "SDL2/sdl.h"

#define X_OFFSET 1
#define Y_OFFSET 1

bool CObjectCollider::IsIntersects(CObject* objectA, CObject* objectB, CollisionSide& side)
{
    SDL_Rect& rectA = objectA->Rect();
    SDL_Rect& rectB = objectB->Rect();

    //Get center
    int CenterAX = objectA->PositionCenter().x;
    int CenterAY = objectA->PositionCenter().y;

    int CenterBX = objectB->PositionCenter().x;
    int CenterBY = objectB->PositionCenter().y;

    float w = 0.5 * (rectA.w + rectB.w);
    float h = 0.5 * (rectA.h + rectB.h);

    float dx = CenterAX - CenterBX;
    float dy = CenterAY - CenterBY;

    int offsetX = X_OFFSET; int offsetY = Y_OFFSET;

    if (abs(dx) < w && abs(dy) < h)
    {
        float wy = w * dy;
        float hx = h * dx;

        if (abs(dx - w) < offsetX || abs(dy - h) < offsetY)
            return false; 

        if (wy > hx)
            side = wy > -hx ? eCSTop : eCSRight;
        else
            side = wy > -hx ? eCSLeft : eCSBottom;

        return true;
    }

    return false;
}

void CObjectCollider::Update()
{
    if (m_object->Type() == eSpawn)
        return;
    
    if (m_object->Type() == eWall)
        return;

    int collide_count = 0;

    for (int i = 0; i < g_Level->MaxObjects(); ++i)
    {
        auto pObj = g_Level->getObject(i);
        if (!pObj) continue;
        if (!pObj->is_Alive()) continue;
        if (pObj->ID() == m_object->ID()) continue;

        CollisionSide collision_side;
        if (IsIntersects(m_object, pObj, collision_side))
        {
            m_object->OnCollide(pObj, collision_side);
            collide_count++;
        }
    }

    if (collide_count == 0 && m_object->IsColliding())
        m_object->AfterCollide();
}