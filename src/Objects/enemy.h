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
    void BuildPathToNode(int GoalNode);
    
    private:
    void MoveTo(Fvector& pos);
    void MoveTo(int NodeID);

    void UpdateMove();
    void UpdateMoveX();
    void UpdateMoveY();
    
    void CheckGoalNode(int& NodeID);
    void Think();
    void SetEnemy();
    bool CanSeeEnemy();

    bool TryResetCollide();
    AINode* GetFreeNode(collision_side collision_side);

    void Search();
    void Attack();

    void SetCollisionEnabled(bool v);
    bool CollisionEnabled() {return m_bCollisionEnabled;};
    bool Colliding() {return m_bIsColliding;};

    bool Stuck();

    //Movement
    Fvector m_target_position;
    Fvector m_previous_position;
    uint8_t m_CollisionSide = 0;
    bool m_bHorizontalMove = false;
    int m_iVelocity;
    int m_GoalNode = 0;
    bool m_bMoving = false;
    bool m_bCollisionEnabled = true;
    CObject* m_objectCollidingWith = NULL;

    //Shooting
    uint8_t m_shoot_cooldown;
    uint32_t m_last_time_shot;
    uint32_t m_move_cooldown;
    uint32_t m_last_time_move;

    //AI
    int m_last_enemies_node = -1;
    CObject *m_enemy = NULL;
    std::vector<int> m_pos_queue;
    
    enum ai_state
    {
        aiSearching = 1,
        aiAttacking = 2,
        aiRunning = 4,
        aiDummy = 8
    };

    ai_state m_current_aistate;
};