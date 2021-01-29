#include "enemy.h"
#include "Systems/engine.h"
#include "Systems/Level/level.h"
#include "systems/engine.h"
#include "objects/object_collider.h"
#include "actor.h"
#include <queue>
#include "globals.h"

#define RUN_HP_THRESHHOLD 20

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

    m_iVelocity = 3;
    SetDirection(eDirDown);
}

void CEnemy::OnCollide(CObject* who_collide, collision_side collision_side)
{
    if (m_bMoving)
    {
        MoveTo(m_pos_queue.front());
    }

    inherited::OnCollide(who_collide, collision_side);
    m_CollisionSide |= collision_side;
}

void CEnemy::AfterCollide()
{
    inherited::AfterCollide();
    m_CollisionSide = CObjectCollider::eCSNone;
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
    Think();
    UpdateMove();
}

void CEnemy::UpdateMove()
{
    if (Position().equals(m_target_position))
    {
        m_bMoving = false;
        if (m_pos_queue.size())
            m_pos_queue.erase(m_pos_queue.begin());
        return;
    }

    m_bHorizontalMove ? UpdateMoveX() : UpdateMoveY();

    m_bMoving = true;
}

void CEnemy::UpdateMoveX()
{
    if (Position().x == m_target_position.x)
    {
        m_bHorizontalMove = false;
        return;
    }

    int velocity = m_iVelocity;
    int dx = abs(Position().x - m_target_position.x);
    if (dx < velocity) velocity = dx;

    if (Position().x < m_target_position.x)
    {
        SetDirection(eDirection::eDirRight);

        if (m_CollisionSide & (collision_side::eCSRight))
            return;

        Position().x += g_iSpeed * velocity;
    }
    else
    {
        SetDirection(eDirection::eDirLeft);

        if (m_CollisionSide & (collision_side::eCSLeft))
            return;

        Position().x -= g_iSpeed * velocity;
    }    
}

void CEnemy::UpdateMoveY()
{
    if (Position().y == m_target_position.y)
    {
        m_bHorizontalMove = true;
        return;
    }

    int velocity = m_iVelocity;
    int dy = abs(Position().y - m_target_position.y);
    if (dy < velocity) velocity = dy;

    if (Position().y < m_target_position.y)
    {
        SetDirection(eDirection::eDirDown);

        if (m_CollisionSide & (collision_side::eCSBottom))
            return;

        Position().y += g_iSpeed * velocity ;
    }
    else
    {
        SetDirection(eDirection::eDirUp);

        if (m_CollisionSide & (collision_side::eCSTop))
            return;

        Position().y -= g_iSpeed * velocity;
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

void CEnemy::MoveTo(int NodeID)
{
    if (!m_nodes.size())
        return;

    int CurrentNodeID = *m_nodes.begin();
    int NextNodeID = NodeID;

    int nodesXcount = static_cast<int>(SCREEN_WIDTH / AI_NODE_WIDTH);

    auto CurrentNode = g_Level->AINodes()[*m_nodes.begin()];
    auto NextNode = g_Level->AINodes()[NextNodeID];

    bool bNeedToRebuildPath = false;

    if (m_bIsColliding)
    {
        m_bIsColliding = false;

        if (m_CollisionSide & collision_side::eCSRight)
        {
            auto pNode = GetFreeNode(collision_side::eCSRight);
            if (pNode) MoveTo(pNode->ID);
        }
        else if (m_CollisionSide & collision_side::eCSLeft)
        {
            auto pNode = GetFreeNode(collision_side::eCSLeft);
            if (pNode) MoveTo(pNode->ID);
        }
        else if (m_CollisionSide & collision_side::eCSTop)
        {
            auto pNode = GetFreeNode(collision_side::eCSTop);
            if (pNode) MoveTo(pNode->ID);
        }
        else if (m_CollisionSide & collision_side::eCSBottom)
        {
            auto pNode = GetFreeNode(collision_side::eCSBottom);
            if (pNode) MoveTo(pNode->ID);
        }

        return;
    }

    if (NextNode->occupied)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "NextNode->occupied %u", ID());
        BuildPathToNode(*m_pos_queue.end());
        return;
    }

    int dt = abs(NextNodeID - CurrentNodeID);
    if (dt > 1 && dt != nodesXcount) //между текущей и следующей нодой есть другие, проверим на занятость
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "dt > 1 && dt != nodesXcount %u", ID());
        if (Direction() & eDirRight)
        {
            int RightNeighbor = CurrentNodeID + 1;
            if (RightNeighbor <= g_Level->AINodes().size() - 1)
            {
                if (g_Level->AINodes()[RightNeighbor]->occupied)
                    bNeedToRebuildPath = true;
            }       
        }
        else if (Direction() & eDirLeft)
        {
            int LeftNeighbor = CurrentNodeID - 1;
            if (LeftNeighbor <= g_Level->AINodes().size() - 1)
            {
                if (g_Level->AINodes()[LeftNeighbor]->occupied)
                    bNeedToRebuildPath = true;
            }
        }
        else if (Direction() & eDirUp)
        {
            int TopNeighbor = CurrentNodeID - nodesXcount;
            if (TopNeighbor <= g_Level->AINodes().size() - 1)
            {
                if (g_Level->AINodes()[TopNeighbor]->occupied)
                    bNeedToRebuildPath = true;
            }
        }
        else if (Direction() & eDirDown)
        {
            int BottomNeighbor = CurrentNodeID + nodesXcount;
            if (BottomNeighbor <= g_Level->AINodes().size() - 1)
            {
                if (g_Level->AINodes()[BottomNeighbor]->occupied)
                    bNeedToRebuildPath = true;
            }
        }
    }

    if (bNeedToRebuildPath)
    {
        BuildPathToNode(*m_pos_queue.end());
        return;
    }

    MoveTo(NextNode->position);
}

void CEnemy::BuildPathToNode(int NodeID)
{
    if (NodeID > g_Level->AINodes().size())
        return;

    if (!m_nodes.size())
        return;

    m_pos_queue.clear();

    int start = (*m_nodes.begin());
    int goal = NodeID;

    CheckGoalNode(goal);

    std::vector<int> frontier;
    frontier.push_back(start);
    
    int size = g_Level->AINodes().size();
    std::vector<bool> bInspected;
    bInspected.resize(1024, 0);

    bInspected[start] = true;

    while (!frontier.empty())
    {
        auto current = frontier.back();
        frontier.pop_back();
        m_pos_queue.push_back(current);

        if (current == goal)
            break;

        for (auto &it : g_Level->AINodes()[current]->neighbors_id)
        {
            if (!g_Level->AINodes()[it]->occupied && !bInspected[it])
            {
                frontier.push_back(g_Level->AINodes()[it]->ID);
                bInspected[it] = true;
                if (it == goal) break;
            }
        }
    }
}

void CEnemy::CheckGoalNode(int& NodeID)
{
    if (g_Level->AINodes()[NodeID]->occupied)
    {
        bool bFoundNewGoal = false;
        for ( auto &it : g_Level->AINodes()[NodeID]->neighbors_id)
        {
            if (!g_Level->AINodes()[it]->occupied)
            {
                bFoundNewGoal = true;
                NodeID = it;
            }
        }

        if (!bFoundNewGoal)
        {
            NodeID = *g_Level->AINodes()[NodeID]->neighbors_id.begin();
            CheckGoalNode(NodeID);
        }
    }
}

AINode* CEnemy::GetFreeNode(collision_side collision_side)
{
    int CurrentNodeID = *m_nodes.begin();
    int nodesXcount = SCREEN_WIDTH / AI_NODE_WIDTH;
    auto Nodes = g_Level->AINodes();
    std::vector<int> free_nodes;
    free_nodes.clear();

    switch (collision_side)
    {
    case collision_side::eCSBottom:
    {
        if (NodeFree(CurrentNodeID - 1)) free_nodes.push_back(CurrentNodeID - 1);
        if (NodeFree(CurrentNodeID + 1)) free_nodes.push_back(CurrentNodeID + 1);
        if (NodeFree(CurrentNodeID - nodesXcount)) free_nodes.push_back(CurrentNodeID - nodesXcount);
    }break;
    case collision_side::eCSTop:
    {
        if (NodeFree(CurrentNodeID - 1)) free_nodes.push_back(CurrentNodeID - 1);
        if (NodeFree(CurrentNodeID + 1)) free_nodes.push_back(CurrentNodeID + 1);
        if (NodeFree(CurrentNodeID + nodesXcount)) free_nodes.push_back(CurrentNodeID + nodesXcount);
    }break;  
    case collision_side::eCSLeft:
    {
        if (NodeFree(CurrentNodeID + 1)) free_nodes.push_back(CurrentNodeID + 1);
        if (NodeFree(CurrentNodeID + nodesXcount)) free_nodes.push_back(CurrentNodeID + nodesXcount);
        if (NodeFree(CurrentNodeID - nodesXcount)) free_nodes.push_back(CurrentNodeID - nodesXcount);
    }break;  
    case collision_side::eCSRight:
    {
        if (NodeFree(CurrentNodeID - 1)) free_nodes.push_back(CurrentNodeID - 1);
        if (NodeFree(CurrentNodeID + nodesXcount)) free_nodes.push_back(CurrentNodeID + nodesXcount);
        if (NodeFree(CurrentNodeID - nodesXcount)) free_nodes.push_back(CurrentNodeID - nodesXcount);
    }break;  
    default: return NULL;
        break;
    }

    if (free_nodes.size())
        return Nodes[free_nodes[rand() % 3]];    
    else
        return NULL;
}

void CEnemy::Think()
{
    if (!m_pos_queue.empty() && !m_bMoving)
    {
        MoveTo(m_pos_queue.front());
    }

    SetEnemy();

    if (CanSeeEnemy())
        Health() > RUN_HP_THRESHHOLD ? Attack() : Run();
    else
        Health() > RUN_HP_THRESHHOLD ? Search() : Run();
}

void CEnemy::SetEnemy()
{

}

bool CEnemy::CanSeeEnemy()
{
    if (!m_enemy)
        return false;

    //handle eyes for enemies here

    return true;
}

void CEnemy::Search()
{

}

void CEnemy::Run()
{
    
}

void CEnemy::Attack()
{
    
}