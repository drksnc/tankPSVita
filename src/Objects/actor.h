#pragma once
#include "object.h"

struct RawObject;

class CActor : virtual public CObject
{
    typedef CObject inherited;

    public:
    CActor();
    virtual ~CActor();
    
    virtual void OnSpawn(RawObject* raw_object);
    virtual void Update();
    virtual bool NeedToRender();
    
    void OnButtonPressed(int button);
    void OnButtonHold(int button);
    void OnButtonReleased(int button);

    int Velocity() {return m_iVelocity;};
    virtual void OnCollide(CObject* who_collide, CObjectCollider::CollisionSide collision_side);
    virtual void AfterCollide();

    private:
    void MoveUp();
    void MoveDown();
    void MoveLeft();
    void MoveRight();

    void Shoot();

    uint8_t m_CollisionSide = 0;
    uint8_t m_shoot_cooldown;
    uint32_t m_last_time_shot;

    protected:
    int m_iVelocity = 1;
    int m_iPreviousVelocity = m_iVelocity;
    void SetVelocity(int value) {m_iPreviousVelocity = m_iVelocity; m_iVelocity = value;};
};

CActor* Actor();