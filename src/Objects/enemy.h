#pragma once

#include "object.h"

class CEnemy : public CObject 
{
    typedef CObject inherited;

    public:
    CEnemy();
    virtual ~CEnemy() {};

    void Shoot();
    virtual void Update();
    virtual void OnSpawn(RawObject* obj);
    void MoveTo(Fvector& pos);

    private:
    void UpdateMove();
    void UpdateMoveX();
    void UpdateMoveY();
    Fvector m_target_position;
    uint8_t m_shoot_cooldown;
    uint32_t m_last_time_shot;
    bool m_bHorizontalMove = false;
    int m_iVelocity;
};