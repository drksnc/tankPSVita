#pragma once

#include "object.h"
#include <queue>

struct AINode;

class CEnemy : public CObject 
{
    typedef CObject inherited;

    public:
    CEnemy();
    virtual ~CEnemy() {};

    virtual void Update();
    virtual void OnSpawn(RawObject* obj);
    virtual void OnCollide(CObject* who_collide, collision_side collision_side);
    virtual void AfterCollide();

    void Shoot();

    void MoveTo(Fvector& pos);
    void MoveTo(int NodeID);

    void BuildPathToNode(int NodeID);
    
    private:
    void UpdateMove();
    void UpdateMoveX();
    void UpdateMoveY();
    
    void CheckGoalNode(int& NodeID);
    void Think();
    void SetEnemy();
    bool CanSeeEnemy();

    AINode* GetFreeNode(collision_side collision_side);

    void Search();
    void Run();
    void Attack();

    Fvector m_target_position;
    uint8_t m_shoot_cooldown;
    uint32_t m_last_time_shot;
    uint8_t m_CollisionSide = 0;
    bool m_bHorizontalMove = false;
    int m_iVelocity;
    CObject *m_enemy = NULL;
    std::vector<int> m_pos_queue;
    bool m_bMoving = false;

    //AI
    int m_last_enemies_node = -1;
};