#include "actor.h"
#include "psp2/ctrl.h"
#include "Systems/engine.h"
#include "Systems/controls.h"
#include "Systems/Utils/vector.h"
#include <sdl2/SDL.h>
#include "Systems/Level/level.h"
#include "systems/Render/render.h"
#include "Objects/bullet.h"

const int g_iSpeed = 1;

typedef CObjectCollider::CollisionSide collision_side;

CActor* g_actor = NULL;
CActor* Actor() {return g_actor;};

CActor::CActor()
{
    m_shoot_cooldown = 100;
    m_last_time_shot = m_shoot_cooldown;
}

CActor::~CActor()
{

}

void CActor::OnSpawn(RawObject* raw_object)
{
    inherited::OnSpawn(raw_object);

    g_Level->SetCurrentControlEntity(this);

    if (g_Level->CurrentControlEntity()->ID() == this->ID())
        g_actor = this;

    SetVelocity(3);
}

void CActor::OnCollide(CObject* who_collide, collision_side collision_side)
{
    inherited::OnCollide(who_collide, collision_side);
    m_CollisionSide |= collision_side;
}

void CActor::AfterCollide()
{
    inherited::AfterCollide();
    m_CollisionSide = CObjectCollider::eCSNone;
}

void CActor::OnButtonPressed(int button)
{
    switch (button)
    {
    case SCE_CTRL_SQUARE: Shoot(); break;
    default:
        break;
    }
}

void CActor::OnButtonHold(int button)
{
    switch (button)
    {
    case SCE_CTRL_UP: MoveUp(); break;
    case SCE_CTRL_DOWN: MoveDown(); break;
    case SCE_CTRL_LEFT: MoveLeft(); break;
    case SCE_CTRL_RIGHT: MoveRight(); break;
    default:
        break;
    }
}

void CActor::OnButtonReleased(int button)
{

}

void CActor::Shoot()
{
    if (CurrentFrame() - m_last_time_shot < m_shoot_cooldown)
        return;

    g_Level->CreateBullet(this);

    m_last_time_shot = CurrentFrame();
}

void CActor::MoveUp()
{
    if (g_Control->ControlData().buttons & (SCE_CTRL_DOWN | SCE_CTRL_LEFT | SCE_CTRL_RIGHT))
        return;

    SetDirection(eDirUp);

    if (IsColliding() && m_CollisionSide & (collision_side::eCSTop))
        return;

    Position().y -= g_iSpeed * m_iVelocity; 
}

void CActor::MoveDown()
{
    if (g_Control->ControlData().buttons & (SCE_CTRL_UP | SCE_CTRL_LEFT | SCE_CTRL_RIGHT))
        return;

    SetDirection(eDirDown);

    if (IsColliding() && m_CollisionSide & (collision_side::eCSBottom))
        return;

    Position().y += g_iSpeed * m_iVelocity; 
}

void CActor::MoveLeft()
{
    if (g_Control->ControlData().buttons & (SCE_CTRL_UP | SCE_CTRL_DOWN | SCE_CTRL_RIGHT))
        return;

    SetDirection(eDirLeft);

    if (IsColliding() && m_CollisionSide & (collision_side::eCSLeft))
        return;

    Position().x -= g_iSpeed * m_iVelocity;
}

void CActor::MoveRight()
{
    if (g_Control->ControlData().buttons & (SCE_CTRL_UP | SCE_CTRL_DOWN | SCE_CTRL_LEFT))
        return;

    SetDirection(eDirRight);

    if (IsColliding() && m_CollisionSide & (collision_side::eCSRight))
        return;

    Position().x += g_iSpeed * m_iVelocity;
}

bool CActor::NeedToRender()
{
    return inherited::NeedToRender();
}

void CActor::Update()
{
    inherited::Update();
}