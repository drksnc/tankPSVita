#include "bullet.h"
#include "Systems/Utils/vector.h"

void CBullet::OnSpawn(RawObject* raw_obj)
{
    inherited::OnSpawn(raw_obj);

    m_iVelocity = 15;
}

void CBullet::Update()
{
    inherited::Update();

    if (OutOfBoundaries())
    {
        Die();
        return;
    }

    if (m_owner_direction & CObject::eDirRight)
        Position().x += m_iVelocity;
    else if (m_owner_direction & CObject::eDirLeft)
        Position().x -= m_iVelocity;
    else if (m_owner_direction & CObject::eDirUp)
        Position().y -= m_iVelocity;
    else if (m_owner_direction & CObject::eDirDown)
        Position().y += m_iVelocity;
}

void CBullet::OnCollide(CObject* who_collide, CObjectCollider::CollisionSide collision_side)
{
    if (who_collide->ID() == m_object->ID())
        return;

    if (m_bHitTarget)
        return;

    inherited::OnCollide(who_collide, collision_side);
    who_collide->OnHit(m_object, 25);
    m_bHitTarget = true;

    Die();
}

bool CBullet::OutOfBoundaries()
{
    if (Position().x < 0)
        return true;

    if (Position().x > SCREEN_WIDTH)
        return true;  

    if (Position().y < 0)
        return true; 

    if (Position().x > SCREEN_HEIGHT)
        return true; 

    return false;
}
