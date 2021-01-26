#include "enemy.h"
#include "Systems/engine.h"
#include "Systems/Level/level.h"
#include "systems/engine.h"

CEnemy::CEnemy()
{
    m_shoot_cooldown = 100;
    m_iVelocity = 0;
    m_last_time_shot = m_shoot_cooldown;
}

void CEnemy::OnSpawn(RawObject* obj)
{
    inherited::OnSpawn(obj);
    m_target_position.set(Position().x, Position().y);

    m_iVelocity = 2;
    MoveTo(Fvector().set(900, 300));
}

void CEnemy::Shoot()
{
    if (!is_Alive())
        return;

    if (CurrentFrame() - m_last_time_shot < m_shoot_cooldown)
        return;

    g_Level->CreateBullet(this);

    m_last_time_shot = CurrentFrame();
}

void CEnemy::Update()
{
    inherited::Update();
    UpdateMove();
    Shoot();
}

void CEnemy::UpdateMove()
{
    if (Position().equals(m_target_position))
        return;

    m_bHorizontalMove ? UpdateMoveX() : UpdateMoveY();
}

void CEnemy::UpdateMoveX()
{
    if (Position().x == m_target_position.x)
    {
        m_bHorizontalMove = false;
        return;
    }

    if (Position().x < m_target_position.x)
    {
        SetDirection(eDirection::eDirRight);
        Position().x += g_iSpeed * m_iVelocity;
    }
    else
    {
        SetDirection(eDirection::eDirLeft);
        Position().x -= g_iSpeed * m_iVelocity;
    }    
}

void CEnemy::UpdateMoveY()
{
    if (Position().y == m_target_position.y)
    {
        m_bHorizontalMove = true;
        return;
    }

    if (Position().y < m_target_position.y)
    {
        SetDirection(eDirection::eDirDown);
        Position().y += g_iSpeed * m_iVelocity ;
    }
    else
    {
        SetDirection(eDirection::eDirUp);
        Position().y -= g_iSpeed * m_iVelocity;
    }    
}

void CEnemy::MoveTo(Fvector& pos)
{
    int dtX = abs(Position().x - m_target_position.x);
    int dtY = abs(Position().y - m_target_position.y);

    if (dtX > dtY)
        m_bHorizontalMove = true;
    else if (dtX < dtY)
        m_bHorizontalMove = false;
    else
        m_bHorizontalMove = static_cast<bool>(std::rand());

    m_target_position.set(pos.x, pos.y);
}