#pragma once
#include "object.h"
#include "Systems/engine.h"
#include "Systems/render/Render.h"
#include "Systems/level/Level.h"
#include "Systems/Utils/vector.h"
#include "globals.h"

class CBullet : public CObject
{
    typedef CObject inherited;

    public:
    CBullet(CObject* owner)
    {
        m_object = owner;
        m_owner_direction = m_object->Direction();
        m_direction_mask = m_owner_direction;
    };
    
    virtual ~CBullet()
    {

    };

    virtual void OnCollide(CObject* who_collide, CObjectCollider::CollisionSide collision_side);
    virtual void OnSpawn(RawObject* raw_obj);
    virtual void Update();

    CObject* Owner() {return m_object;};

    private:
    bool OutOfBoundaries();

    bool m_bHitTarget = false;
    uint8_t m_owner_direction;
    CObject* m_object;
    uint8_t m_iVelocity = 1;
};