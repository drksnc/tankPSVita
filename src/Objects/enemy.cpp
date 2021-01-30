#include "enemy.h"
#include "Systems/engine.h"
#include "Systems/Level/level.h"
#include "systems/engine.h"
#include "objects/object_collider.h"
#include "actor.h"
#include <queue>
#include <unordered_map>
#include "globals.h"

#define SEE_ENEMY_THRESHHOLD 250

CEnemy::CEnemy()
{
    m_shoot_cooldown = 100;
    m_move_cooldown = 50;
    m_iVelocity = 0;
    m_last_time_shot = m_shoot_cooldown;
    m_current_aistate = aiDummy;
    m_last_time_move = m_move_cooldown;
}

void CEnemy::OnSpawn(RawObject* obj)
{
    inherited::OnSpawn(obj);
    m_target_position.set(Position().x, Position().y);

    m_iVelocity = 3;
    m_current_aistate = aiSearching;
    SetDirection(eDirUp);
}

void CEnemy::OnCollide(CObject* who_collide, collision_side collision_side)
{
    if (!CollisionEnabled())
        return;

    inherited::OnCollide(who_collide, collision_side);
    m_objectCollidingWith = who_collide;
    m_CollisionSide |= collision_side;
}

void CEnemy::AfterCollide()
{
    if (!CollisionEnabled())
        return;

    inherited::AfterCollide();
    m_objectCollidingWith = NULL;
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

    if (!m_pos_queue.empty() && !m_bMoving)
        MoveTo(m_pos_queue.front());

    Think();
    UpdateMove();
}

void CEnemy::UpdateMove()
{
    if (Stuck()) //застряли
        TryResetCollide();

    if (Position().equals(m_target_position)) //приехали в точку
    {
        m_bMoving = false;
        if (CollisionEnabled()) //всё ок, запланированное место
        {
            if (m_pos_queue.size())
                m_pos_queue.erase(m_pos_queue.begin());
        }
        else //вырвались из чужой коллизии, просто вернём свою
        {
            SetCollisionEnabled(true);
        }

        return;
    }

    m_bHorizontalMove ? UpdateMoveX() : UpdateMoveY();
    m_bMoving = true;
    m_previous_position = Position();
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
    m_last_time_move = CurrentFrame();

    int dtX = abs(Position().x - m_target_position.x);
    int dtY = abs(Position().y - m_target_position.y);

    if (dtX > dtY)
        m_bHorizontalMove = true;
    else if (dtX < dtY)
        m_bHorizontalMove = false;
    else
    {   
        int CurrentNodeID = GetNodeByPosition(Position());
        int nodesXcount = g_Level->m_nodesXcount;

        m_bHorizontalMove = static_cast<bool>(std::rand());

        if (Position().y - m_target_position.y > 0) //стоим выше
        {
            if (NodeFree(CurrentNodeID + nodesXcount))
                m_bHorizontalMove = false;
            else
                m_bHorizontalMove = true;
        }
        else if (Position().y - m_target_position.y < 0) //стоим ниже
        {
            if (NodeFree(CurrentNodeID - nodesXcount))
                m_bHorizontalMove = false;
            else
                m_bHorizontalMove = true;
        }

        if (Position().x - m_target_position.x > 0) //стоим левее
        {
            if (NodeFree(CurrentNodeID + 1))
                m_bHorizontalMove = true;
            else
                m_bHorizontalMove = false;
        }
        else if (Position().x - m_target_position.x < 0)
        {
            if (NodeFree(CurrentNodeID - 1))
                m_bHorizontalMove = true;
            else
                m_bHorizontalMove = false;
        }
    }

    m_target_position.set(pos.x, pos.y);
}

void CEnemy::MoveTo(int NodeID)
{
    if (!m_nodes.size())
        return;

    int CurrentNodeID = *m_nodes.begin();
    int NextNodeID = NodeID;

    int nodesXcount = g_Level->m_nodesXcount;

    auto CurrentNode = g_Level->AINodes()[*m_nodes.begin()];
    auto NextNode = g_Level->AINodes()[NextNodeID];

    if (NextNode->occupied)
    {
        BuildPathToNode(m_GoalNode);
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
    m_GoalNode = goal;

    typedef std::pair<int, int> PQElement;
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> frontier;
    frontier.emplace(0, start);
    std::unordered_map<int, int> cost_so_far;
    cost_so_far.clear();
    cost_so_far[start] = 0;

    while (!frontier.empty())
    {
        auto current = frontier.top().second;
        frontier.pop();
        m_pos_queue.push_back(current);

        if (current == goal)
            break;

        for (auto &it : g_Level->AINodes()[current]->neighbors_id)
        {
            if (g_Level->AINodes()[it]->occupied)
                continue;

            auto iterCost = cost_so_far.find(it);
            int new_cost = cost_so_far[it] + abs(current - it) == 1 ? 10 : 15;
            
            if (iterCost == cost_so_far.end() || !(*iterCost).second || new_cost < cost_so_far[it])
            {
                cost_so_far[it] = new_cost;
                int priority = new_cost + Fvector().heuristic(g_Level->AINodes()[it]->position, g_Level->AINodes()[goal]->position);
                frontier.emplace(priority, it);
            }
        }
    }
}

void CEnemy::CheckGoalNode(int& NodeID)
{
    if (g_Level->AINodes()[NodeID]->occupied)
    {
        bool bNodeFound = false;
        for ( auto &it : g_Level->AINodes()[NodeID]->neighbors_id)
        {
            if (!g_Level->AINodes()[it]->occupied)
            {
                bNodeFound = true;
                NodeID = it;
            }
        }

        if (!bNodeFound)
        {
            NodeID = *g_Level->AINodes()[NodeID]->neighbors_id.begin();
            CheckGoalNode(NodeID);
        }
    }
}

AINode* CEnemy::GetFreeNode(collision_side collision_side)
{
    int CurrentNodeID = *m_nodes.begin();
    int nodesXcount = g_Level->m_nodesXcount;
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
        return Nodes[free_nodes[rand() % free_nodes.size()]];    
    else
        return NULL;
}

bool CEnemy::TryResetCollide()
{
    AINode* pTargetNode = NULL;
    if (m_CollisionSide & (collision_side::eCSRight))
        pTargetNode = GetFreeNode(collision_side::eCSRight);
    else if (m_CollisionSide & (collision_side::eCSLeft))
        pTargetNode = GetFreeNode(collision_side::eCSLeft);
    else if (m_CollisionSide & (collision_side::eCSTop))
        pTargetNode = GetFreeNode(collision_side::eCSTop);
    else if (m_CollisionSide & (collision_side::eCSBottom))
        pTargetNode = GetFreeNode(collision_side::eCSBottom);

    if (!pTargetNode)
        return false;

    SetCollisionEnabled(false);
    MoveTo(pTargetNode->ID);
    return true;
}

void CEnemy::SetCollisionEnabled(bool v)
{
    m_bIsColliding = v;
    m_bCollisionEnabled = v;
    m_CollisionSide = collision_side::eCSNone;
}

bool CEnemy::Stuck()
{
    if (!m_bMoving)
        return false; 

    if (!Colliding())
        return false;
    
    if (m_objectCollidingWith && m_objectCollidingWith->Static())
        return false;

    if (!m_previous_position.equals(Position()))
        return false;
    
    return true;
}

void CEnemy::Think()
{
    SetEnemy();
    CanSeeEnemy() ? Attack() : Search();
}

void CEnemy::SetEnemy()
{
    if (!Actor())
        m_enemy = NULL;

    m_enemy = Actor();
}

bool CEnemy::CanSeeEnemy()
{
    if (!m_enemy)
        return false;

    if (Position().distance_to(m_enemy->Position()) > SEE_ENEMY_THRESHHOLD)
        return false;

    return true;
}

void CEnemy::Search()
{
    if (m_current_aistate == aiSearching)
    {
        if (!m_pos_queue.size() || GetNodeByPosition(Position()) == *m_pos_queue.end())
        {
            if (m_last_enemies_node != -1)
            {
                BuildPathToNode(m_last_enemies_node);
                m_last_enemies_node = -1;
            }

            int NextNode = std::rand() % g_Level->AINodes().size() - 1;
            while (g_Level->AINodes()[NextNode]->occupied)
                NextNode = std::rand() % g_Level->AINodes().size() - 1;      

            BuildPathToNode(NextNode);
        }
    }
    else
    {
        BuildPathToNode(m_last_enemies_node);
    }

    m_current_aistate = aiSearching;
}

void CEnemy::Attack()
{
    //попытаемся встать на одну линию с врагом
    m_last_enemies_node = GetNodeByPosition(m_enemy->Position());

    int dX = abs(m_enemy->Position().x - Position().x);
    int dY = abs(m_enemy->Position().y - Position().y);

    //сильно дёргается, пришлось сделать кулдаун на передвижение 
    if (CurrentFrame() - m_last_time_move > m_move_cooldown)
    {
        if (dX > dY)
        {
            if (dX > m_enemy->Rect().w)
                MoveTo(Fvector().set(m_enemy->Position().x, Position().y));
        }
        else
        {
            if (dY > m_enemy->Rect().h)
                MoveTo(Fvector().set(Position().x, m_enemy->Position().y));
        }

        m_last_time_move = CurrentFrame();
    }

    
    if (!m_bMoving)
    {
        int enemynode = GetNodeByPosition(m_enemy->Position());
        int currentnode = GetNodeByPosition(PositionCenter());
        int nextYNodeOffset = g_Level->m_nodesXcount;
        int nextXNodeOffset = 1;

        if (dX < dY) 
        {
            if (Direction() & eDirRight || Direction() & eDirLeft)
                return;

            if (m_enemy->Position().y > Position().y)
            {
                //проверим все клетки до врага, если их невозможно разрушить, то объедем
                for (int i = 0; i < (abs(currentnode - enemynode)) / g_Level->m_nodesXcount; ++i) 
                {
                    auto nodeToCheck = g_Level->AINodes()[currentnode + nextYNodeOffset];
                    if (nodeToCheck->occupied && nodeToCheck->object_inside && !nodeToCheck->object_inside->Breakable())
                    {
                        BuildPathToNode(GetNodeByPosition(m_enemy->Position()));
                        return;
                    }
                    nextYNodeOffset += g_Level->m_nodesXcount;
                }
                SetDirection(eDirDown);
            }
            else
            {
                for (int i = 0; i < (abs(currentnode - enemynode)) / g_Level->m_nodesXcount; ++i)
                {
                    auto nodeToCheck = g_Level->AINodes()[currentnode - nextYNodeOffset];
                    if (nodeToCheck->occupied && nodeToCheck->object_inside && !nodeToCheck->object_inside->Breakable())
                    {
                        BuildPathToNode(GetNodeByPosition(m_enemy->Position()));
                        return;
                    }
                    nextYNodeOffset += g_Level->m_nodesXcount;
                }
                SetDirection(eDirUp);
            }
        }
        else
        {
            if (Direction() & eDirUp || Direction() & eDirDown)
                return;

            if (m_enemy->Position().x > Position().x)
            {
                for (int i = 0; i < abs(currentnode - enemynode); ++i)
                {
                    auto nodeToCheck = g_Level->AINodes()[currentnode + nextXNodeOffset];
                    if (nodeToCheck->occupied && nodeToCheck->object_inside && !nodeToCheck->object_inside->Breakable())
                    {
                        BuildPathToNode(GetNodeByPosition(m_enemy->Position()));
                        return;
                    }
                    nextXNodeOffset += 1;
                }
                SetDirection(eDirRight);
            }
            else
            {
                for (int i = 0; i < abs(currentnode - enemynode); ++i)
                {
                    auto nodeToCheck = g_Level->AINodes()[currentnode - nextXNodeOffset];
                    if (nodeToCheck->occupied && nodeToCheck->object_inside && !nodeToCheck->object_inside->Breakable())
                    {
                        BuildPathToNode(GetNodeByPosition(m_enemy->Position()));
                        return;
                    }
                    nextXNodeOffset += 1;
                }
                SetDirection(eDirLeft);
            }
        }

        Shoot();
    }
    
    m_current_aistate = aiAttacking;
}