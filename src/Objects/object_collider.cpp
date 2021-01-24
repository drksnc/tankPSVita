#include "object_collider.h"
#include "Systems/Level/level.h"
#include "Systems/Render/render.h"
#include "object.h"
#include "actor.h"
#include "globals.h"
#include "Systems/engine.h"
#include "SDL2/sdl.h"

bool CObjectCollider::IsCollide(CObject* objectA, CObject* objectB)
{
    SDL_Rect& rectA = objectA->Rect();
    SDL_Rect& rectB = objectB->Rect();

    int offset = objectA->ColliderOffset();
ssssss
    int LeftA = rectA.x;
    int BottomA = rectA.y + rectA.h;
    int RightA = rectA.x + rectA.w;
    int TopA = rectA.y;

    int LeftB = rectB.x;
    int BottomB = rectB.y + rectB.h;
    int RightB = rectB.x + rectB.w;
    int TopB = rectB.y;

    if (BottomA + offset <= TopB) return false;
    if (RightA - offset <= LeftB) return false;
    if (TopA + offset >= BottomB) return false;
    if (LeftA - offset >= RightB) return false;

    return true;
}

void CObjectCollider::Update()
{
    for (int i = 0; i < 255; ++i)
    {
        auto obj = g_Level->getObject(i);
        if (!obj) continue;
        if (obj->ID() == m_object->ID()) continue;

        if (IsCollide(m_object, obj))
        {
            m_object->OnCollide(obj);
        }
    }
}