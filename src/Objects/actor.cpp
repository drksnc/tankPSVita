#include "actor.h"
#include "psp2/ctrl.h"
#include "Systems/engine.h"
#include "Systems/controls.h"
#include "Systems/Utils/vector.h"
#include <sdl2/SDL.h>
#include "Systems/Level/level.h"

const int g_iSpeed = 1;

CActor* g_actor = NULL;
CActor* Actor() {return g_actor;};

CActor::CActor()
{

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

void CActor::OnCollide(CObject* who_collide)
{
    m_bColliding = true;
    inherited::OnCollide(who_collide);
}

void CActor::OnButtonPressed(int button)
{
    
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

void CActor::MoveUp()
{
    if (g_Control->ControlData().buttons & (SCE_CTRL_DOWN | SCE_CTRL_LEFT | SCE_CTRL_RIGHT))
        return;

    SetDirection(eDirUp);
    Position().y -= g_iSpeed * m_iVelocity; 
}

void CActor::MoveDown()
{
    if (g_Control->ControlData().buttons & (SCE_CTRL_UP | SCE_CTRL_LEFT | SCE_CTRL_RIGHT))
        return;

    SetDirection(eDirDown);
    Position().y += g_iSpeed * m_iVelocity; 
}

void CActor::MoveLeft()
{
    if (g_Control->ControlData().buttons & (SCE_CTRL_UP | SCE_CTRL_DOWN | SCE_CTRL_RIGHT))
        return;
    
    SetDirection(eDirLeft);
    Position().x -= g_iSpeed * m_iVelocity;
}

void CActor::MoveRight()
{
    if (g_Control->ControlData().buttons & (SCE_CTRL_UP | SCE_CTRL_DOWN | SCE_CTRL_LEFT))
        return;
    
    SetDirection(eDirRight);
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