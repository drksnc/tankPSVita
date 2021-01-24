#include "object_collider.h"
#include "Systems/Level/level.h"
#include "Systems/Render/render.h"
#include "object.h"
#include "actor.h"
#include "globals.h"
#include "Systems/engine.h"
#include "SDL2/sdl.h"

bool CObjectCollider::IsIntersects(CObject* objectA, CObject* objectB, CollisionSide& side)
{
    SDL_Rect& rectA = objectA->Rect();
    SDL_Rect& rectB = objectB->Rect();

    //Get center
    int CenterAX = objectA->Position().x + objectA->Rect().w / 2;
    int CenterAY = objectA->Position().y + objectA->Rect().h / 2;

    int CenterBX = objectB->Position().x + objectB->Rect().w / 2;
    int CenterBY = objectB->Position().y + objectB->Rect().h / 2;

    float w = 0.5 * (rectA.w + rectB.w);
    float h = 0.5 * (rectA.h + rectB.h);

    float dx = CenterAX - CenterBX;
    float dy = CenterAY - CenterBY;

    int offsetX = 0; int offsetY = 0;

    if (abs(dx) <= w - offsetX && abs(dy) <= h - offsetY)
    {
        float wy = w * dy;
        float hx = h * dx;

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
    int collide_count = 0;

    for (int i = 0; i < g_Level->MaxObjects(); ++i)
    {
        auto obj = g_Level->getObject(i);
        if (!obj) continue;
        if (!obj->is_Alive()) continue;
        if (obj->ID() == m_object->ID()) continue;

        CollisionSide collision_side;
        if (IsIntersects(m_object, obj, collision_side))
        {
            m_object->OnCollide(obj, collision_side);
            collide_count++;
        }
    }

    if (collide_count == 0 && m_object->IsColliding())
        m_object->AfterCollide();
}